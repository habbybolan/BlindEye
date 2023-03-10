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
	virtual bool CancelState() override;
};

// Moving to target state 
class BLINDEYE_API FMovingState : public FAbilityState
{
public:
	FMovingState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
	virtual bool CancelState() override;
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

	UPROPERTY(EditDefaultsOnly, Category=Target, meta=(ToolTip="Offset the target position when hovering above ground"))
	float TargetPositionOffset = 0.f;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	TArray<TEnumAsByte<EObjectTypeQuery>> EnemyObjectTypes;

	UPROPERTY(EditDefaultsOnly,Category=Pull)
	TSubclassOf<UBaseDamageType> DamageType;
	 
	UPROPERTY(EditDefaultsOnly, Category=Target)
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectBlocker;

	UPROPERTY(EditDefaultsOnly, Category=Target)
	TSubclassOf<ACrowRushTarget> TargetType;

	UPROPERTY(EditDefaultsOnly, Category=Target)
	float StepSize = 50;
 
	UPROPERTY(EditDefaultsOnly, Category=Target)
	float StepOffsetTowardsPlayer = 50;

	UPROPERTY(EditDefaultsOnly)
	float DamageAmount = 20;

	UPROPERTY(EditDefaultsOnly, Category=Movement) 
	float UpdateMovementDelay = 0.02;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TEnumAsByte<EEasingFunc::Type> EasingFunction;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LandingAnim;
	
	void ApplyDamage();

	UFUNCTION(Client, Reliable)
	void CLI_StartAiming();

	void StartMovement();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartMovementHelper(FVector StartPos, FVector CalculatedEndPos);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_ResetPlayerState();

	UFUNCTION(Client, Reliable)
	void CLI_RemoveTarget();

	FTimerHandle UpdateTargetTimerHandle;
	FTimerHandle CheckIsLandedTimerHandle;

protected:

	FVector StartingPosition; 
	FVector EndPosition;
	float CalculatedDuration;

	void UpdateTargetPosition();
	
	UFUNCTION()
	void CheckIsLanded(); 
	bool CheckIsLandedHelper();
	

	FVector CalculateTargetPosition();

	UFUNCTION()
	void UpdatePlayerMovement();
	
	FTimerHandle UpdatePlayerTimerHandle; 
	float CurrDuration = 0;

	void SetAsLanded();
	UFUNCTION()
	void SetLandingAnimFinished(UAnimMontage* Montage, bool bInterrupted);

	virtual void EndAbilityLogic() override;

	bool FindValidTargetLocation(FVector& CurrLoc, FVector EndLoc, FVector DirBackAlongLine, uint8 StepNum);

	UPROPERTY()
	ACrowRushTarget* Target;
};
