// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "ShrineIsland.h"
#include "GameFramework/Actor.h"
#include "IslandManager.generated.h"

UCLASS()
class BLINDEYE_API AIslandManager : public AActor
{
	GENERATED_BODY()
	
public:

	uint8 GetNumOfIslands(); 
	const TArray<AIsland*> GetActiveIslands();
	AIsland* GetIslandOfID(uint8 islandID);
	AShrineIsland* GetShrineIsland();

protected:
	
	AIslandManager();
	
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AIsland*> ActiveIslands; 
 
	UPROPERTY()
	TArray<AIsland*> InactiveIslands;

	UPROPERTY()
	AShrineIsland* ShrineIsland;

};
