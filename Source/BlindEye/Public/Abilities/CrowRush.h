// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "CrowRushTarget.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "Kismet/KismetMathLibrary.h"
#include "CrowRush.generated.h"

// Aiming start state
class BLINDEYE_API FAimingStartState : public FAbilityState
{
public:
	FAimingStartState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

// Moving to target state 
class BLINDEYE_API FMovingState : public FAbilityState
{
public:
	FMovingState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

// End State
class BLINDEYE_API FEndState : public FAbilityState
{ 
public:
	FEndState(AAbilityBase* ability);
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
	float DurationAtMaxDistance = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxDistance = 1500.f;

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACrowRushTarget> TargetType;

	UPROPERTY(EditDefaultsOnly) 
	float UpdateMovementDelay = 0.02;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectBlocker;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EEasingFunc::Type> EasingFunction;
	
	void ResetPlayerSpeed();

	void StartAiming();
	void StartMovement();

protected:

	FVector StartingPosition; 
	FVector EndPosition;
	float CalculatedDuration;

	void UpdateTargetPosition();
	FTimerHandle UpdateTargetTimerHandle;

	FVector CalculateTargetPosition();

	UFUNCTION()
	void UpdatePlayerMovement();
	
	FTimerHandle UpdatePlayerTimerHandle; 
	float CurrDuration = 0;

	virtual void EndAbilityLogic() override;

	UPROPERTY()
	ACrowRushTarget* Target;
};
