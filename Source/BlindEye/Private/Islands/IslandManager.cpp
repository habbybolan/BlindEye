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

// Called when the game starts or when spawned
void AIslandManager::BeginPlay()
{
	Super::BeginPlay();

	uint8 Index = 0;

	// Cache all child islands and give them IDs
	UWorld* World = GetWorld();
	if (World)
	{
		// Cache all surrounding islands
		TArray<AActor*> AllIslands;
		UGameplayStatics::GetAllActorsOfClass(World, AIsland::StaticClass(), AllIslands);
		for (AActor* IslandActor : AllIslands)
		{
			if (AIsland* Island = Cast<AIsland>(IslandActor))
			{
				Island->Initialize(Index++);
				ActiveIslands.Add(Island);
			}
		}

		// Cache shrine island
		AActor* ShrineIslandActor = UGameplayStatics::GetActorOfClass(World, AShrineIsland::StaticClass());
		check(ShrineIslandActor);
		ShrineIsland = Cast<AShrineIsland>(ShrineIslandActor);
	}
	
}

