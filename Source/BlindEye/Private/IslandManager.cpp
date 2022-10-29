// Copyright (C) Nicholas Johnson 2022


#include "IslandManager.h"

#include "Island.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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

// Called when the game starts or when spawned
void AIslandManager::BeginPlay()
{
	Super::BeginPlay();

	uint8 Index = 0;

	// Cache all child islands and give them IDs
	UWorld* World = GetWorld();
	if (World)
	{ 
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
	}
	
}

