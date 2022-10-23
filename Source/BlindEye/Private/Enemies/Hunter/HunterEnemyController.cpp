// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemyController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AHunterEnemyController::AHunterEnemyController()
{
}

void AHunterEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	TArray<AActor*> OutTriggerVolumes;
	UGameplayStatics::GetAllActorsOfClass(World, ABurrowerTriggerVolume::StaticClass(), OutTriggerVolumes);
	for (AActor* VolumeActor : OutTriggerVolumes)
	{
		ABurrowerTriggerVolume* BurrowerVolume = Cast<ABurrowerTriggerVolume>(VolumeActor);
		TriggerVolumes.Add(BurrowerVolume->IslandType, BurrowerVolume);
		BurrowerVolume->OnActorBeginOverlap.AddDynamic(this, &AHunterEnemyController::SetEnteredNewIsland);
	}
	
	World->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AHunterEnemyController::SpawnHunter, SpawnDelay, false);
}

void AHunterEnemyController::SetEnteredNewIsland(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Hunter == nullptr) return;
	if (OtherActor == Hunter)
	{
		CurrIsland = Cast<ABurrowerTriggerVolume>(OverlappedActor);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Red, "Hunter entered: " + UEnum::GetValueAsString(CurrIsland->IslandType));
	}
}

void AHunterEnemyController::SetAlwaysVisible(bool IsAlwaysVisible)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (GameState)
	{
		if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState))
		{
			BlindEyeGameState->bHunterAlwaysVisible = IsAlwaysVisible;
			if (Hunter)
			{
				Hunter->TrySetVisibility(true);
			}
		}
	}
}

bool AHunterEnemyController::CanJumpAttack(AActor* Target)
{
	return !IsJumpAttackOnDelay && IsInJumpAttackRange(Target);
}

void AHunterEnemyController::PerformJumpAttack()
{
	IsJumpAttackOnDelay = true;
	GetBlackboardComponent()->SetValueAsBool("bAttacking", true);
	Hunter->PerformJumpAttack();
	GetWorldTimerManager().SetTimer(JumpAttackDelayTimerHandle, this, &AHunterEnemyController::SetCanBasicAttack, JumpAttackDelay, false);
}

void AHunterEnemyController::DebugSpawnHunter()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (Hunter) return;

	SpawnHunter();
	World->GetTimerManager().ClearTimer(SpawnDelayTimerHandle);
}

void AHunterEnemyController::TrySetVisibility(bool visibility)
{
	if (!Hunter) return;
	Hunter->TrySetVisibility(visibility);
}

void AHunterEnemyController::UpdateMovementSpeed(EHunterStates NewHunterState)
{
	if (!Hunter) return;
	Hunter->UpdateMovementSpeed(NewHunterState);
}

bool AHunterEnemyController::IsInJumpAttackRange(AActor* Target)
{
	if (GetPawn() == nullptr) return false;
	
	float Distance = FVector::Distance(Target->GetActorLocation(), GetPawn()->GetActorLocation());
	return Distance < DistanceToJumpAttack;
}

void AHunterEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	Hunter = Cast<AHunterEnemy>(InPawn);
	
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Hunter))
	{
		HealthInterface->GetHealthComponent()->OnDeathDelegate.AddUFunction(this, FName("OnHunterDeath"));
	}

	// Get random player to attack
	AGameStateBase* GameState = UGameplayStatics::GetGameState(world);
	AActor* RandPlayerTarget;
	if (GameState->PlayerArray.Num() == 1)
	{
		RandPlayerTarget = GameState->PlayerArray[0]->GetPawn();
	}
	else
	{
		RandPlayerTarget = GameState->PlayerArray[UKismetMathLibrary::RandomIntegerInRange(0, 1)]->GetPawn();
	}
	
	InitializeBehaviorTree();
	GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("bDead", false);

	InPawn->OnTakeAnyDamage.AddDynamic(this, &AHunterEnemyController::OnTakeDamage);
}

void AHunterEnemyController::SetCanBasicAttack()
{
	IsJumpAttackOnDelay = false;
}

void AHunterEnemyController::OnHunterDeath(AActor* HunterKilled)
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AHunterEnemyController::SpawnHunter, SpawnDelay, false);
	Hunter = nullptr;

	GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("bDead", false);
	GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("IsFirstRun", false);
	GetBrainComponent()->StopLogic(TEXT("HunterDeath"));
}

void AHunterEnemyController::SpawnHunter() 
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(World, ABurrowerSpawnPoint::StaticClass(),SpawnPoints);
	if (SpawnPoints.Num() == 0) return;
	AActor* SpawnPoint = SpawnPoints[UKismetMathLibrary::RandomIntegerInRange(0, SpawnPoints.Num() - 1)];
	
	FVector SpawnLocation = SpawnPoint->GetActorLocation();
	FRotator Rotation = SpawnPoint->GetActorRotation();
	FActorSpawnParameters params; 
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AHunterEnemy* SpawnedHunter = World->SpawnActor<AHunterEnemy>(HunterType, SpawnLocation, Rotation, params);

	bool bAlwaysVisible = false;
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (GameState)
	{
		if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState))
		{
			bAlwaysVisible = BlindEyeGameState->bHunterAlwaysVisible;
		}
	}
	
	// if debugger for always visible, spawn hunter visible
	if (bAlwaysVisible)
	{
		SpawnedHunter->TrySetVisibility(true);
	}
	
	Possess(SpawnedHunter);
}

void AHunterEnemyController::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->SetValueAsBool("bDamaged", true);
	}
}
