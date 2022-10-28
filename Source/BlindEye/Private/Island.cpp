// Copyright (C) Nicholas Johnson 2022


#include "Island.h"

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EmptyRoot = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(EmptyRoot);
	IslandTrigger = CreateDefaultSubobject<UBurrowerTriggerVolume>("Trigger Volume");
	IslandTrigger->SetupAttachment(EmptyRoot);
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AllChildActors;
	GetAllChildActors(AllChildActors, true);
	for (AActor* ChildActor : AllChildActors)
	{
		// cache all owned burrower spawn points
		if (UBurrowerSpawnPoint* BSpawnPoint = Cast<UBurrowerSpawnPoint>(ChildActor))
		{
			BSpawnPoint->IslandID = IslandID;
			OwnedBurrowerSpawnPoints.Add(BSpawnPoint);
		}
	}

	IslandTrigger->IslandID = IslandID;
	
}

