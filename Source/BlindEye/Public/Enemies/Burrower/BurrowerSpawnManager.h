// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
#include "BurrowerSpawnPoint.h"
#include "BurrowerTriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Interfaces/BurrowerSpawnManagerListener.h"
#include "BurrowerSpawnManager.generated.h"

UCLASS()
class BLINDEYE_API ABurrowerSpawnManager : public AActor, public IBurrowerSpawnManagerListener
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABurrowerSpawnManager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnDelay = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABurrowerEnemy> BurrowerType;
 
	void SpawnBurrower();
 
	TArray<ABlindEyePlayerCharacter*> GetPlayersOnIsland(uint8 islandID) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle SpawnTimerHandle;

	TMap<uint8, TArray<ABurrowerEnemy*>> SpawnedBurrowers;
	UPROPERTY()
	TMap<uint8, UBurrowerTriggerVolume*> BurrowerTriggerVolumes;

	void InitializeMaps();

	UFUNCTION()
	void OnBurrowerDeath(AActor* BurrowerActor);
	
	UBurrowerSpawnPoint* FindRandomSpawnPoint();
	void CacheSpawnPoints();

	UFUNCTION()
	void TriggerVolumeOverlapped(UPrimitiveComponent* OverlappedActor, AActor* OtherActor);
	UFUNCTION()  
	void TriggerVolumeLeft(UPrimitiveComponent* EndOverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void PlayerEnteredIsland(ABlindEyePlayerCharacter* Player, uint8 IslandType);
 
	UFUNCTION()
	void PlayerLeftIsland(ABlindEyePlayerCharacter* Player, uint8 islandID);
	
	TMap<uint8, TArray<UBurrowerSpawnPoint*>> BurrowerSpawnPoints;

};
