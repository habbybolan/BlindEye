// Copyright (C) Nicholas Johnson 2022


#include "Islands/IslandSpawnPoint.h"

AIslandSpawnPoint::AIslandSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<UStaticMeshComponent>("Root");
	RootComponent = Root;
}

void AIslandSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

