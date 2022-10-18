// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
#include "BurrowerSpawnPoint.h"
#include "GameFramework/Actor.h"
#include "BurrowerSpawnManager.generated.h"

UCLASS()
class BLINDEYE_API ABurrowerSpawnManager : public AActor
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle SpawnTimerHandle;

	TMap<EIslandPosition, TArray<ABurrowerEnemy*>> SpawnedBurrowers;

	void InitializeMaps();

	UFUNCTION()
	void OnBurrowerDeath(AActor* BurrowerActor);
	
	ABurrowerSpawnPoint* FindRandomSpawnPoint();
	void CacheSpawnPoints();

	UFUNCTION()
	void TriggerVolumeOverlapped(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()  
	void TriggerVolumeLeft(AActor* EndOverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void PlayerEnteredIsland(ABlindEyePlayerCharacter* Player, EIslandPosition IslandType);
 
	UFUNCTION()
	void PlayerLeftIsland(ABlindEyePlayerCharacter* Player, EIslandPosition IslandType);
	
	TMap<EIslandPosition, TArray<ABurrowerSpawnPoint*>> BurrowerSpawnPoints;

};
