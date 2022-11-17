// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/BurrowerSpawnManagerListener.h"
#include "BurrowerSpawnManager.generated.h"

class AIslandManager;
class AIsland;
class UBurrowerTriggerVolume;
class UBurrowerSpawnPoint;
class ABurrowerEnemy;

UCLASS()
class BLINDEYE_API ABurrowerSpawnManager : public AActor, public IBurrowerSpawnManagerListener
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABurrowerSpawnManager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABurrowerEnemy> BurrowerType;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxNumBurrowersPerIsland = 3;
 
	void SpawnBurrowerRandLocation(); 
	void SpawnBurrower(AIsland* Island);
	void SpawnBurrowerAtBurrowerSpawnPoint(UBurrowerSpawnPoint* SpawnPoint, AIsland* Island);

	// Helper for performing spawning logic of burrower at island and general location
	ABurrowerEnemy* SpawnBurrowerHelper(FVector SpawnLocation, FRotator SpawnRotation, AIsland* Island);
 
	TArray<ABlindEyePlayerCharacter*> GetPlayersOnIsland(uint8 islandID) override;

	void SetInBurstState();

	// Spawns a single burrower for intro enemy tutorial for burrowers and snappers
	ABurrowerEnemy* TutorialBurrowerSpawn();

	void KillAllBurrowers();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnStartingTutorialEnded();

	// Called after level loaded
	UFUNCTION()
	void Initialize();

	TMap<uint8, TArray<ABurrowerEnemy*>> SpawnedBurrowers;

	void InitializeMaps();

	UFUNCTION()
	void OnBurrowerDeath(AActor* BurrowerActor);
	
	UBurrowerSpawnPoint* FindRandomUnusedSpawnPoint();

	UFUNCTION()
	void TriggerVolumeOverlapped(UPrimitiveComponent* OverlappedActor, AActor* OtherActor);
	UFUNCTION()  
	void TriggerVolumeLeft(UPrimitiveComponent* EndOverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void PlayerEnteredIsland(ABlindEyePlayerCharacter* Player, uint8 IslandType);
 
	UFUNCTION()
	void PlayerLeftIsland(ABlindEyePlayerCharacter* Player, uint8 islandID);

	UFUNCTION()
	void NewIslandAdded(AIsland* Island);

	void SubscribeToIsland(AIsland* Island);

	UPROPERTY()
	AIslandManager* IslandManager;

};
