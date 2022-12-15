// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "CrowFlurry.generated.h"

class BLINDEYE_API FPerformCrowFlurryState : public FAbilityState
{
public:  
	FPerformCrowFlurryState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void ExitState() override;
	virtual bool CancelState() override;
};

class BLINDEYE_API FCancelCrowFlurryState : public FAbilityState
{
public:  
	FCancelCrowFlurryState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void ExitState() override;
	virtual bool CancelState() override;
};

class BLINDEYE_API FEndCrowFlurryState : public FAbilityState
{
public:  
	FEndCrowFlurryState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void ExitState() override;
	virtual bool CancelState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACrowFlurry : public AAbilityBase
{
	GENERATED_BODY()

public:
	ACrowFlurry();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagePerSec = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Width = 400;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Height = 150;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Range = 400;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float CostPercentPerSec = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float InitialCostPercent = 5; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> FirstHitDamageType;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> AlreadyHitDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=1))
	float CrowFlurryLerpSpeed = 0.15;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=540))
	float RotationSpeedInFlurry = 100;

	UPROPERTY(EditDefaultsOnly)
	float CrowFlurryDamageDelay = 0.2f;
 
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=1))
	float MovementSlowDuringFlurry = 0.5;
	
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=1))
	float MovementSlowAfterStoppingFlurry = 0.5;
 
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* CrowFlurryAnimation;

	bool bFlurryActive = false;
	
	void StartCrowFlurry();
	void PlayAbilityAnimation();
	void PlayAnimationHelper();
	UFUNCTION(NetMulticast, Unreliable)
	void MULT_PlayAbilityAnimation();
	void EndAbilityAnimation();

	FTimerHandle CrowFlurryTimerHandle;
	FTimerHandle DurationTimerHandle;
	// For calculating crow flurry rotation lerping to control rotation
	FTimerHandle CalculateRotationTimerHandle;
	FTimerHandle RotateFlurryTimerHandle;

protected:
	
	float CalcRotationDelay = 0.05f;


	UFUNCTION(NetMulticast, Unreliable)
	void MULT_RotateFlurry(); 
 
	UFUNCTION(BlueprintImplementableEvent)
	void RotateFlurryHelper();

	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator CurrFlurryRotation;

	TSet<TWeakObjectPtr<AActor>> MarkedEnemies;

	UFUNCTION()
	void CrowFlurryDurationEnd();

	UFUNCTION()
	void PerformCrowFlurry();

	UFUNCTION()
	void CalcFlurryRotation();

	UFUNCTION()
	void UseAnimNotifyExecuted();

	UFUNCTION()
	void AbilityAnimationEnded();
	
	virtual void EndAbilityLogic() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
