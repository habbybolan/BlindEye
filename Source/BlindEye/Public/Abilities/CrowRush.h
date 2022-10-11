// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "CrowRush.generated.h"

// Dash State
class BLINDEYE_API FCrowRushStartState : public FAbilityState
{
public:
	FCrowRushStartState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACrowRush : public AAbilityBase
{
	GENERATED_BODY()

public:
	ACrowRush();

	UPROPERTY(EditDefaultsOnly)
	float DashDuration = 0.8f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	float DashSpeedIncrease = 10.f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	float DashAccelerationIncrease = 10.f;

	UPROPERTY(EditDefaultsOnly, Category=Pull)
	float PullSphereRadius = 100.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MinKnockTowardsCenterForce = 200.f;
	 
	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MaxKnockTowardsCenterForce = 400.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MinKnockUpToCenterForce = 100.f;
 
	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MaxKnockUpToCenterForce = 200.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MinKnockTowardsEndForce = 200.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MaxKnockTowardsEndForce = 400.f;
   
	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MinKnockUpToEndForce = 100.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	float MaxKnockUpToEndForce = 200.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> EnemyObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	float DamageAmount = 20;

	void UpdatePlayerSpeed(); 
	void ResetPlayerSpeed();

protected:

	FVector StartingPosition;
	
};
