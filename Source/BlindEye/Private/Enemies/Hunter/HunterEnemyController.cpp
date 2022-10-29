// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemyController.h"

#include "BrainComponent.h"
#include "Islands/Island.h"
#include "Islands/IslandManager.h"
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
	if (World)
	{
		World->OnWorldBeginPlay.AddUFunction(this, TEXT("Initialize"));
	}
}

void AHunterEnemyController::Initialize()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
	check(BlindEyeGS)
	IslandManager = BlindEyeGS->GetIslandManager();
	check(IslandManager)

	IslandManager->IslandAddedDelegate.AddDynamic(this, &AHunterEnemyController::NewIslandAdded);

	TArray<AIsland*> Islands = IslandManager->GetActiveIslands();
	for (AIsland* Island : Islands)
	{
		NewIslandAdded(Island);
	}
	IslandManager->GetShrineIsland()->IslandTrigger->CustomOverlapStartDelegate.AddDynamic(this, &AHunterEnemyController::SetEnteredNewIsland);
	
	World->GetTimerManager().SetTimer(InitialSpawnDelayTimerHandle, this, &AHunterEnemyController::SpawnHunter, InitialSpawnDelay, false);
}

void AHunterEnemyController::NewIslandAdded(AIsland* Island)
{
	Island->IslandTrigger->CustomOverlapStartDelegate.AddDynamic(this, &AHunterEnemyController::SetEnteredNewIsland);
}

void AHunterEnemyController::SetEnteredNewIsland(UPrimitiveComponent* OverlappedActor, AActor* OtherActor)
{
	if (Hunter == nullptr) return;
	if (OtherActor == Hunter)
	{
		CurrIsland = Cast<UBurrowerTriggerVolume>(OverlappedActor);
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
	World->GetTimerManager().ClearTimer(InitialSpawnDelayTimerHandle);
	World->GetTimerManager().ClearTimer(ReturnDelayTimerHandle);
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

void AHunterEnemyController::OnStunStart(float StunDuration)
{
	if (Hunter)
	{
		Hunter->OnStunStart(StunDuration);
	}
}

void AHunterEnemyController::OnStunEnd()
{
	if (Hunter)
	{
		Hunter->SetFleeing();
		UWorld* World = GetWorld();
		if (ensure(World))
		{
			World->GetTimerManager().SetTimer(InvisDelayTimerHandle, this, &AHunterEnemyController::StunInvisDelayFinished, 1, false);
		}
	}	 
}

void AHunterEnemyController::DespawnHunter()
{
	CachedHealth = Hunter->GetHealth();
	UnPossess();
	Hunter->Destroy();
	RemoveHunterHelper();
}

void AHunterEnemyController::StunInvisDelayFinished()
{
	DespawnHunter(); 
	DelayedReturn(AfterStunReturnDelay);
}

void AHunterEnemyController::TargetKilledInvisDelayFinished()
{
	DespawnHunter(); 
	DelayedReturn(AfterKillingPlayerDelay);
}

void AHunterEnemyController::MULT_SetCachedHealth_Implementation()
{
	if (Hunter)
	{
		Hunter->BP_OnTakeDamage(0, FVector::ZeroVector, nullptr, nullptr);
	}
}

void AHunterEnemyController::DelayedReturn(float ReturnDelay)
{
	UWorld* World = GetWorld();
	if (ensure(World))
	{
		World->GetTimerManager().SetTimer(ReturnDelayTimerHandle, this, &AHunterEnemyController::SpawnHunter, ReturnDelay, false);
	}
}

void AHunterEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	Hunter = Cast<AHunterEnemy>(InPawn);
	if (CachedHealth > 0)
	{
		Hunter->SetHealth(CachedHealth);
		MULT_SetCachedHealth();
	}

	// Get random player to attack
	ABlindEyeGameState* GameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(world));
	check(GameState);
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
		CurrIsland = GameState->GetIslandManager()->GetActiveIslands()[0]->IslandTrigger;
	}
}
UBurrowerTriggerVolume* AHunterEnemyController::CheckIslandSpawnedOn()
{
	if (Hunter == nullptr) return nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr) return nullptr;

	TArray<AActor*> OutIslandVolumes; 
	if (UKismetSystemLibrary::SphereOverlapActors(World, Hunter->GetActorLocation(), 50, IslandTriggerObjectType,
		nullptr, TArray<AActor*>(), OutIslandVolumes))
	{
		return Cast<UBurrowerTriggerVolume>(OutIslandVolumes[0]);
	}
	return nullptr;
}

void AHunterEnemyController::OnHunterDeath(AActor* HunterKilled)
{
	CachedHealth = Hunter->GetMaxHealth();
	DelayedReturn(AfterDeathReturnDelay);
	RemoveHunterHelper();
	
}

void AHunterEnemyController::RemoveHunterHelper()
{
	Hunter = nullptr;
	GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("IsFirstRun", false);
	GetBrainComponent()->StopLogic(TEXT("HunterDeath"));
}

void AHunterEnemyController::StartChanneling()
{
	if (Hunter)
	{
		Hunter->StartChanneling();
	}
}

void AHunterEnemyController::OnMarkedPlayerDeath()
{
	// Find other alive players
	UWorld* World = GetWorld();
	if (!World) return;
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (GameState)
	{
		TArray<APlayerState*> Players = GameState->PlayerArray;
		for (APlayerState* FoundPlayerState : Players)
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(FoundPlayerState->GetPawn());
			check(Player);
			if (!Player->GetIsDead())
			{
				Hunter->SetFleeing();
				SetBTTarget(Player);
				World->GetTimerManager().SetTimer(InvisDelayTimerHandle, this, &AHunterEnemyController::TargetKilledInvisDelayFinished, 1, false);
			}
		}
	}
}

void AHunterEnemyController::SpawnHunter() 
{
	UWorld* World = GetWorld();
	if (!World) return;

	AIsland* RandIsland = IslandManager->GetActiveIslands()[UKismetMathLibrary::RandomIntegerInRange(0, IslandManager->GetActiveIslands().Num() - 1)];
	UBurrowerSpawnPoint* RandSpawnPoint = RandIsland->GetBurrowerSpawnPoints()[UKismetMathLibrary::RandomIntegerInRange(0, RandIsland->GetBurrowerSpawnPoints().Num() - 1)];
	
	FVector SpawnLocation = RandSpawnPoint->GetComponentLocation();
	FRotator Rotation = RandSpawnPoint->GetComponentRotation();
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
