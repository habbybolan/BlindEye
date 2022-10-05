// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "Boids/BasicAttackSmallFlock.h"
#include "SharedBasicAbility.generated.h"

// Send out first flock
class BLINDEYE_API UFirstAttackState : public FAbilityState
{
public:
	UFirstAttackState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

// Sends out second flock
class BLINDEYE_API USecondAttackState : public FAbilityState
{
public:
	USecondAttackState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

// Sends out last flock
class BLINDEYE_API ULastAttackState : public FAbilityState
{
public:
	ULastAttackState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

UCLASS()
class BLINDEYE_API ASharedBasicAbility : public AAbilityBase
{
	GENERATED_BODY()

public:
	ASharedBasicAbility();

	void SetLeaveAbilityTimer();
	void ClearLeaveAbilityTimer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABasicAttackSmallFlock> FirstChargeFlockType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABasicAttackSmallFlock> SecondChargeFlockType;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABasicAttackSmallFlock> LastChargeFlockType;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> ChargeAnimMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChargeDelay1 = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChargeDelay2 = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FirstChargeCostPercent = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SecondChargeCostPercent = 5;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ThirdChargeCostPercent = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AbilityCancelDelay = 2;

	uint8 CurrCharge = 0;
	
	UFUNCTION(Server, Reliable)
	void SpawnFlock();

	bool bIsAttacking = false;

	// Wait for ability use animation notify to send out flock
	void PlayAbilityAnimation();
	UFUNCTION()
	void UseAnimNotifyExecuted();

	// Wait for animation to end to goto next charge
	void WaitForEndAbilityNotify();
	UFUNCTION()
	void EndAnimNotifyExecuted();  
	
protected:
	bool AbilityTest = true;
	FTimerHandle ResetAbilityTimerHandle;
	

	virtual void TryCancelAbility() override;
	virtual void EndAbilityLogic() override;

	
};




