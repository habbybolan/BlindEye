// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "BurrowerEnemy.generated.h"

class ASnapperEnemy;
class UHealthComponent;

enum class EBurrowActionState
{
	Spawning,
	Attacking
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABurrowerEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:

	ABurrowerEnemy();

	// Spawn the burrower at a point, and either attack or spawn snappers depending on action state
	void SpawnAction(FTransform SpawnLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MinSnappersSpawn = 2;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxSnappersSpawn = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesTraceSpawner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASnapperEnemy> SnapperType;

	FOnTimelineFloat SpawnUpdateEvent; 
	FOnTimelineEvent SpawnFinishedEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* SpawnTimelineComponent;

	EBurrowActionState ActionState;

protected:

	virtual void BeginPlay() override;
	
	TArray<ASnapperEnemy*> SpawnedSnappers;
	
	TArray<FVector> GetSnapperSpawnPoints();

	// Plays on update on spawning timeline playing
	UFUNCTION()
	void TimelineSpawnMovement();

	// On Spawn timeline finishing
	UFUNCTION()
	void TimelineSpawnFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SpawnCurve;

	FVector CachedSpawnLocation;
	
	UFUNCTION()
	void SpawnSnappers();
	
};
