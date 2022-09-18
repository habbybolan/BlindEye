// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemyController.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ABurrowerEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	CachedPreviousActions.SetNum(3, false);

	InitializeBehaviorTree();
	CacheSpawnPoints();
}

EBurrowActionState ABurrowerEnemyController::GetCurrAction()
{
	return CachedPreviousActions[0];
}

void ABurrowerEnemyController::CalcNewActionState()
{
	if (CachedPreviousActions[0] == EBurrowActionState::Spawning)
	{
		AddNewActionState(EBurrowActionState::Attacking);
	} else
	{
		AddNewActionState(EBurrowActionState::Spawning);
	}
}

void ABurrowerEnemyController::SpawnActionStart()
{
	AddNewActionState(EBurrowActionState::Spawning);
	FTransform randSpawn = FindRandSpawnPoint();
	if (CachedBurrower)
	{
		CachedBurrower->SpawnAction(randSpawn);
	}
}

void ABurrowerEnemyController::AttackActionStart()
{
	AddNewActionState(EBurrowActionState::Attacking);
	if (CachedBurrower)
	{
		CachedBurrower->AttackAction(GetRandomPlayerForTarget());
	} 
}

void ABurrowerEnemyController::CacheSpawnPoints()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerSpawnPoint::StaticClass(), SpawnLocation);
}

FTransform ABurrowerEnemyController::FindRandSpawnPoint()
{
	if (SpawnLocation.Num() == 0) return FTransform();
	uint32 randIndex = UKismetMathLibrary::RandomInteger(SpawnLocation.Num());
	return SpawnLocation[randIndex]->GetTransform();
}

void ABurrowerEnemyController::AddNewActionState(EBurrowActionState NewAction)
{
	for (int i = CachedPreviousActions.Num() - 1; i > 0; i--)
	{
		CachedPreviousActions[i] = CachedPreviousActions[i - 1];
	}
	CachedPreviousActions[0] = NewAction;
}

ABlindEyePlayerCharacter* ABurrowerEnemyController::GetRandomPlayerForTarget() const
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;

	// TODO: All players should be retrieved from GameState (Store all connected players for easy retrieval)
	// Get random player character
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(world, ABlindEyePlayerCharacter::StaticClass(), AllPlayers);
	uint32 rand = UKismetMathLibrary::RandomIntegerInRange(0, AllPlayers.Num() - 1);
	return Cast<ABlindEyePlayerCharacter>(AllPlayers[rand]);
}

void ABurrowerEnemyController::ActionStateFinished()
{
	// TODO: Change - currently just flip-flopping between states
	if (CachedPreviousActions[0] == EBurrowActionState::Spawning)
	{
		AttackActionStart();
	} else
	{
		SpawnActionStart();
	}
}

void ABurrowerEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;
	//world->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABurrowerEnemyController::SpawnLogic, 5.0f, true);
	CachedBurrower->SetHidden(true);
	//SpawnActionStart();

	CachedBurrower->ActionStateFinished.BindUFunction(this, FName("ActionStateFinished"));
}
