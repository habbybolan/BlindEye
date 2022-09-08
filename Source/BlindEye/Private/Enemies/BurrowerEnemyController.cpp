// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BurrowerEnemyController.h"

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
	SpawnLogic();

	CachedBurrower->ActionStateFinished.BindUFunction(this, FName("ActionStateFinished"));
}

void ABurrowerEnemyController::SpawnLogic()
{
	FTransform randSpawn = FindRandSpawnPoint();
	if (CachedBurrower)
	{
		CachedBurrower->SpawnAction(randSpawn);
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

void ABurrowerEnemyController::ActionStateFinished()
{
	SpawnLogic();
}
