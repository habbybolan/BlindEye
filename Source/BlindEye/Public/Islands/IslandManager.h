// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "IslandSpawnPoint.h"
#include "ShrineIsland.h"
#include "GameFramework/Actor.h"
#include "IslandManager.generated.h"

UCLASS()
class BLINDEYE_API AIslandManager : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditInstanceOnly)
	TArray<AIslandSpawnPoint*> IslandSpawnPoints; 

	uint8 GetNumOfIslands(); 
	const TArray<AIsland*> GetActiveIslands();
	AIsland* GetIslandOfID(uint8 islandID);
	AShrineIsland* GetShrineIsland();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnFinishSignature, AIsland*, Island);
	UPROPERTY()
	FSpawnFinishSignature IslandAddedDelegate; 

	UFUNCTION()
	void ActivateNextIsland();

protected:
	
	AIslandManager();
	
	virtual void BeginPlay() override;

	FTimerHandle TempSpawnIslandTimer;

	UPROPERTY()
	TArray<AIsland*> ActiveIslands; 
 
	UPROPERTY()
	TArray<AIsland*> InactiveIslands;

	UPROPERTY()
	AShrineIsland* ShrineIsland;

	void CacheShrineIsland(uint8& index);
	void CacheOuterIslands(uint8& index);
	void CacheSpawnPoints();

	UFUNCTION()
	void IslandSpawningFinished(AIsland* Island);

};
