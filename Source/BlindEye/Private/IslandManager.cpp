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
	return CachedIslands.Num();
}

const TArray<AIsland*> AIslandManager::GetIslands()
{
	return CachedIslands;
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
				Island->IslandID = Index++;
				CachedIslands.Add(Island);
			}
		}
	}
	
}

