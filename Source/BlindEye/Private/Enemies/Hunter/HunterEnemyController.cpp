// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemyController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "GameFramework/CharacterMovementComponent.h"
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

void AHunterEnemyController::PerformChargedJump()
{
	if (Hunter)
	{
		Hunter->PerformChargedJump();
	}
}

void AHunterEnemyController::PerformBasicAttack()
{
	if (Hunter)
	{
		Hunter->PerformBasicAttack();
	}
}

bool AHunterEnemyController::CanChargedJump(AActor* Target)
{
	if (Hunter == nullptr) return false;
	
	// TODO: Check if visible sight to player and no obstacles in the way	
	return !Hunter->GetIsChargedJumpOnCooldown() &&
			IsInChargedJumpRange(Target) &&
			IsOnSameIslandAsPlayer(Target) &&
			Hunter->GetIsCharged() &&
			!Hunter->GetCharacterMovement()->IsFalling() &&
			!Hunter->GetIsAttacking();
}

bool AHunterEnemyController::CanBasicAttack(AActor* Target)
{
	if (Hunter == nullptr) return false;
	
	return  IsInBasicAttackRange(Target) &&
			IsOnSameIslandAsPlayer(Target) &&
			!Hunter->GetIsAttacking();
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

bool AHunterEnemyController::IsInChargedJumpRange(AActor* Target)
{
	if (Hunter)
	{
		float Distance = FVector::Distance(Target->GetActorLocation(), Hunter->GetActorLocation());
		return Distance < Hunter->MaxDistanceToChargeJump &&  Distance > Hunter->MinDistanceToChargeJump;
	}
	return false;
}

bool AHunterEnemyController::IsInBasicAttackRange(AActor* Target)
{
	if (Hunter)
	{
		float Distance = FVector::Distance(Target->GetActorLocation(), Hunter->GetActorLocation());
		return Distance < Hunter->MaxDistanceForBasicAttack;
	}
	return false;
}

bool AHunterEnemyController::IsOnSameIslandAsPlayer(AActor* Target)
{
	for (ABlindEyePlayerCharacter* Player : CurrIsland->GetPlayerActorsOverlapping())
	{
		if (Player == Target) return true;
	}
	return false;
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
	GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("bFirstRun", true);

	InPawn->OnTakeAnyDamage.AddDynamic(this, &AHunterEnemyController::OnTakeDamage);

	// Initial check for which island Hunter spawned on
	CurrIsland = CheckIslandSpawnedOn();
	if (CurrIsland == nullptr)
	{
		CurrIsland = TriggerVolumes[EIslandPosition::IslandA];
	}
}

ABurrowerTriggerVolume* AHunterEnemyController::CheckIslandSpawnedOn()
{
	if (Hunter == nullptr) return nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr) return nullptr;

	TArray<AActor*> OurActors;
	if (UKismetSystemLibrary::SphereOverlapActors(World, Hunter->GetActorLocation(), 50, IslandTriggerObjectType,
		nullptr, TArray<AActor*>(), OurActors))
	{
		return Cast<ABurrowerTriggerVolume>(OurActors[0]);
	}
	return nullptr;
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
