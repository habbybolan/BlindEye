// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
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

protected:
	
	AIslandManager();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly)
	TArray<AIsland*> ActiveIslands; 
 
	UPROPERTY(EditInstanceOnly)
	TArray<AIsland*> InactiveIslands;
	

};
