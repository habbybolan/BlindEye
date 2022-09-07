// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BurrowerEnemyController.h"

#include "Enemies/BurrowerEnemy.h"

void ABurrowerEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	if (ABurrowerEnemy* burrower = Cast<ABurrowerEnemy>(GetPawn()))
	{
		world->GetTimerManager().SetTimer(SpawnTimerHandle, burrower, &ABurrowerEnemy::SpawnSnappers, 10.0f, true);
	}
}
