// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Island.generated.h"

UCLASS()
class BLINDEYE_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();
	
	UPROPERTY()
	USceneComponent* EmptyRoot;

	// guarantee at least one burrower spawn point
	UPROPERTY(EditDefaultsOnly)
	UBurrowerSpawnPoint* BurrowerSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	UBurrowerTriggerVolume* IslandTrigger;

	UPROPERTY(EditDefaultsOnly)
	uint8 NumIslands = 2;

	uint8 IslandID;

	void Initialize(uint8 islandID);
	TArray<UBurrowerSpawnPoint*> GetBurrowerSpawnPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UBurrowerSpawnPoint*> OwnedBurrowerSpawnPoints;

};
