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

	CacheSpawnPoints();
	
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	world->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABurrowerEnemyController::SpawnLogic, 5.0f, true);
	CachedBurrower->SetHidden(true);
	SpawnLogic();
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
