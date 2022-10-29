// Copyright (C) Nicholas Johnson 2022


#include "Islands/IslandManager.h"

#include "Islands/Island.h"
#include "Islands/ShrineIsland.h"
#include "Kismet/GameplayStatics.h"

AIslandManager::AIslandManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

uint8 AIslandManager::GetNumOfIslands()
{
	return ActiveIslands.Num();
}

const TArray<AIsland*> AIslandManager::GetActiveIslands()
{
	return ActiveIslands;
}

AIsland* AIslandManager::GetIslandOfID(uint8 islandID)
{
	for (AIsland* Island : ActiveIslands)
	{
		if (Island->IslandID == islandID) return Island;
	}
	return nullptr;
}

AShrineIsland* AIslandManager::GetShrineIsland()
{
	return ShrineIsland;
}

void AIslandManager::ActivateNextIsland()
{
	check(IslandSpawnPoints.Num() > 0);

	// spawn if any inactive islands
	if (InactiveIslands.Num() > 0)
	{
		AIsland* Island = InactiveIslands[0];
		InactiveIslands.RemoveAt(0);

		// TODO: Get random spawn point
		Island->SpawnIsland(IslandSpawnPoints[0]->GetActorLocation());
		Island->SpawnFinishedDelegate.AddDynamic(this, &AIslandManager::IslandSpawningFinished);
	}
}

void AIslandManager::IslandSpawningFinished(AIsland* Island)
{
	Island->SpawnFinishedDelegate.Remove(this, TEXT("IslandSpawningFinished"));
	ActiveIslands.Add(Island);
}

// Called when the game starts or when spawned
void AIslandManager::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	check(World)

	uint8 Index = 0;

	// Cache shrine island
	AActor* ShrineIslandActor = UGameplayStatics::GetActorOfClass(World, AShrineIsland::StaticClass());
	check(ShrineIslandActor);
	ShrineIsland = Cast<AShrineIsland>(ShrineIslandActor);
	ShrineIsland->Initialize(Index++);

	// Cache all child islands and give them IDs
	{
		// Cache all surrounding islands
		TArray<AActor*> AllIslands;
		UGameplayStatics::GetAllActorsOfClass(World, AIsland::StaticClass(), AllIslands);
		for (AActor* IslandActor : AllIslands)
		{
			if (AIsland* Island = Cast<AIsland>(IslandActor))
			{
				Island->Initialize(Index++);
				if (Island->bActive)
				{
					ActiveIslands.Add(Island);
				} else
				{
					InactiveIslands.Add(Island);
				}
			}
		}
	}
	
	// Get all Island spawn points
	TArray<AActor*> SpawnPointActors;
	UGameplayStatics::GetAllActorsOfClass(World, AIslandSpawnPoint::StaticClass(), SpawnPointActors);
	for (AActor* SpawnPointActor : SpawnPointActors)
	{
		IslandSpawnPoints.Add(Cast<AIslandSpawnPoint>(SpawnPointActor));
	}

	World->GetTimerManager().SetTimer(TempSpawnIslandTimer, this, &AIslandManager::ActivateNextIsland, 5.0f, true);
}

