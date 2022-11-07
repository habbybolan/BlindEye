// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseIsland.h"
#include "Island.generated.h"

class UBurrowerSpawnPoint; 
class ABurrowerSpawnManager;
class UHunterSpawnPoint;

UCLASS()
class BLINDEYE_API AIsland : public ABaseIsland
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

	UPROPERTY(EditInstanceOnly)
	uint8 Priority = 0;

	UPROPERTY(EditInstanceOnly)
	bool bActive = true;

	// guarantee at least one burrower spawn point
	UPROPERTY(EditDefaultsOnly)
	UBurrowerSpawnPoint* BurrowerSpawnPoint;

	// Guarantee at least one hunter spawn point
	UPROPERTY(EditDefaultsOnly)
	UHunterSpawnPoint* HunterSpawnPoint;

	virtual void Initialize(uint8 islandID) override;
	TArray<UBurrowerSpawnPoint*> GetBurrowerSpawnPoints();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnFinishSignature, AIsland*, Island);
	UPROPERTY()
	FSpawnFinishSignature SpawnFinishedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnIsland(FVector startLocation);
	bool GetIsActive();

	UBurrowerSpawnPoint* GetRandUnusedBurrowerSpawnPoint();

	UHunterSpawnPoint* GetRandHunterSpawnPoint();

	FORCEINLINE bool operator<(const AIsland &Other) const
	{
		return Priority < Other.Priority;
	}
	
protected:
	virtual void BeginPlay() override;
	
	bool bSpawning = false;

	UPROPERTY(BlueprintReadOnly)
	FVector StartLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector EndLocation;

	UPROPERTY()
	TArray<UBurrowerSpawnPoint*> OwnedBurrowerSpawnPoints;
 
	UPROPERTY()
	TArray<UHunterSpawnPoint*> OwnedHunterSpawnPoints;

	UFUNCTION(BlueprintCallable)
	void IslandFinishedSpawning();

	void Disable(bool bDisabled);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StartLevelMovement();

};
