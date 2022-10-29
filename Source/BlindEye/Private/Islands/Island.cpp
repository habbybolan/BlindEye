// Copyright (C) Nicholas Johnson 2022


#include "Islands/Island.h"

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BurrowerSpawnPoint = CreateDefaultSubobject<UBurrowerSpawnPoint>("Burrower Spawn Point");
	BurrowerSpawnPoint->SetupAttachment(EmptyRoot);
}

void AIsland::Initialize(uint8 islandID)
{
	Super::Initialize(islandID);
	TSet<UActorComponent*> AllComponents = GetComponents();
	for (auto ChildActor : AllComponents)
	{
		// cache all owned burrower spawn points
		if (UBurrowerSpawnPoint* BSpawnPoint = Cast<UBurrowerSpawnPoint>(ChildActor))
		{
			BSpawnPoint->IslandID = islandID;
			OwnedBurrowerSpawnPoints.Add(BSpawnPoint);
		}
	}
}

TArray<UBurrowerSpawnPoint*> AIsland::GetBurrowerSpawnPoints()
{
	return OwnedBurrowerSpawnPoints;
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();
}

