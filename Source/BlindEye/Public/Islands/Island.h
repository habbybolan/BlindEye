// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseIsland.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "Island.generated.h"

UCLASS()
class BLINDEYE_API AIsland : public ABaseIsland
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

	// guarantee at least one burrower spawn point
	UPROPERTY(EditDefaultsOnly)
	UBurrowerSpawnPoint* BurrowerSpawnPoint;

	virtual void Initialize(uint8 islandID) override;
	TArray<UBurrowerSpawnPoint*> GetBurrowerSpawnPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UBurrowerSpawnPoint*> OwnedBurrowerSpawnPoints;

};
