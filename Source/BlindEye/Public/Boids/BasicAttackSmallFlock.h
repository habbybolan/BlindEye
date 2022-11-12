// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Boids/Flock.h"
#include "BasicAttackSmallFlock.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABasicAttackSmallFlock : public AFlock
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TargetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	float UpForceOnTargetReached = 100000.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxTargetSeekingStrengthIncrease = 3;

	UPROPERTY(EditDefaultsOnly)
	float DistToApplyTargetSeekingIncrease = 300;

	UPROPERTY(EditDefaultsOnly, Category=Shrinking)
	float DistToPlayerToStartShrinking = 300;

	UPROPERTY(EditDefaultsOnly, Category=Shrinking)
	float DistFromPlayerToFullyShrink = 500; 

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=1))
	float MovementPercentAfterReachingTarget = 0.75f;

	FName BoneSpawnLocation;

	FVector InitialTarget;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	float BaseSeekingStrength;

	FTimerHandle FlockCheckTimerHandle;
	float FlockCheckDelay = 0.02;

	void FlockCheck();

	void CheckForDamage();
	void CheckTargetReached();

	void CheckReturnedToPlayer();
	void CheckShrinking();
	
	void GoBackToPlayer();

	void SendEachBoidUp();

	UPROPERTY()
	bool bHasReachedTarget = false;

	void SetTargets(const TArray<FVector>& TargetLocation);
};
