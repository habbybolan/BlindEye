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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetDistanceFromInstigator = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TargetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> SpawnLineCastObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	float UpForceOnTargetReached = 100000.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxTargetSeekingStrengthIncrease = 3;

	UPROPERTY(EditDefaultsOnly)
	float DistToApplyTargetSeekingIncrease = 300;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	float BaseSeekingStrength;

	void CheckForDamage();
	void CheckGoBackToPlayer();
	void CheckReturnedToPlayer();
	
	void SendEachBoidUp();

	bool bHasReachedTarget = false;
	
};
