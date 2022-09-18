// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle SpawnTimerHandle;
	
	UPROPERTY()
	TMap<uint32, TWeakObjectPtr<ABurrowerEnemy>> SpawnedBurrowers;

	UFUNCTION()
	void OnBurrowerDeath(AActor* BurrowerActor);

	void SpawnBurrower();
	FTransform FindRandomSpawnPoint();
	void CacheSpawnPoints();

	UPROPERTY()
	TArray<AActor*> SpawnLocation;

};
