// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "BurrowerEnemy.generated.h"

class ASnapperEnemy;
class UHealthComponent;

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
	float SpawnTimeAppearingLength = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesTraceSpawner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASnapperEnemy> SnapperType;

	FOnTimelineFloat SpawnUpdateEvent; 
	FOnTimelineEvent SpawnFinishedEvent;

	FOnTimelineFloat HideUpdateEvent; 
	FOnTimelineEvent HideFinishedEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* SpawnTimelineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* HideTimelineComponent;

	DECLARE_DELEGATE(FActionStateFinishedSignature)
	FActionStateFinishedSignature ActionStateFinished;

protected:

	virtual void BeginPlay() override;
	
	TArray<ASnapperEnemy*> SpawnedSnappers;
	
	TArray<FVector> GetSnapperSpawnPoints();

	// Timeline functions for burrower appearing

	UFUNCTION()
	void TimelineSpawnMovement();

	UFUNCTION()
	void TimelineSpawnFinished();

	// Timeline functions for hiding burrower

	UFUNCTION()
	void TimelineHideMovement();

	UFUNCTION()
	void TimelineHideFinished();

	UFUNCTION()
	void StartHideLogic();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SpawnCurve;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* HideCurve;

	FTimerHandle HideTimerHandle;
	FVector CachedSpawnLocation;

	FVector CachedBeforeHidePosition;
	
	UFUNCTION()
	void SpawnSnappers();
	
};
