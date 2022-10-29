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

	UPROPERTY(EditInstanceOnly)
	bool bActive = true;

	// guarantee at least one burrower spawn point
	UPROPERTY(EditDefaultsOnly)
	UBurrowerSpawnPoint* BurrowerSpawnPoint;

	virtual void Initialize(uint8 islandID) override;
	TArray<UBurrowerSpawnPoint*> GetBurrowerSpawnPoints();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnFinishSignature, AIsland*, Island);
	UPROPERTY()
	FSpawnFinishSignature SpawnFinishedDelegate;

	void SpawnIsland(FVector StartLocation);
	bool GetIsActive();

protected:
	virtual void BeginPlay() override;
	
	bool bSpawning = false;
	FVector CachedTargetPosition;

	UPROPERTY()
	TArray<UBurrowerSpawnPoint*> OwnedBurrowerSpawnPoints;

	UFUNCTION(BlueprintCallable)
	void IslandFinishedSpawning();

	void Disable(bool bDisabled);

};
