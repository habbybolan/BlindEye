// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BurrowerEnemyController.h"

#include "Characters/BlindEyeCharacter.h"
#include "Enemies/BurrowerEnemy.h"
#include "Enemies/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ABurrowerEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	CachedPreviousActions.SetNum(3, false);

	CacheSpawnPoints();
	
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	//world->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABurrowerEnemyController::SpawnLogic, 5.0f, true);
	CachedBurrower->SetHidden(true);
	SpawnActionStart();

	CachedBurrower->ActionStateFinished.BindUFunction(this, FName("ActionStateFinished"));
}

EBurrowActionState ABurrowerEnemyController::GetCurrAction()
{
	return CachedPreviousActions[0];
}

void ABurrowerEnemyController::SpawnActionStart()
{
	AddNewActionState(EBurrowActionState::Spawning);
	FTransform randSpawn = FindRandSpawnPoint();
	if (CachedBurrower)
	{
		CachedBurrower->SpawnAction(randSpawn);
		CachedBurrower->SetHidden(false);
	}
}

void ABurrowerEnemyController::AttackActionStart()
{
	AddNewActionState(EBurrowActionState::Attacking);
	if (CachedBurrower)
	{
		CachedBurrower->AttackAction(GetRandomPlayerForTarget());
		CachedBurrower->SetHidden(false);
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

ABlindEyeCharacter* ABurrowerEnemyController::GetRandomPlayerForTarget() const
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;

	// TODO: All players should be retrieved from GameState (Store all connected players for easy retrieval)
	// Get random player character
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(world, ABlindEyeCharacter::StaticClass(), AllPlayers);
	uint32 rand = UKismetMathLibrary::RandomIntegerInRange(0, AllPlayers.Num() - 1);
	return Cast<ABlindEyeCharacter>(AllPlayers[rand]);
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
