// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "CrowCocoon.generated.h"

class BLINDEYE_API UCrowCocoonStart : public FAbilityState
{ 
public:  
	UCrowCocoonStart(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

class BLINDEYE_API UCrowPulseState : public FAbilityState
{ 
public:  
	UCrowPulseState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACrowCocoon : public AAbilityBase
{
	GENERATED_BODY()

public:

	ACrowCocoon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick1 = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick2 = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick3 = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick4 = 50;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BirdMeterPercentLossPerSec = 20; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHoldDuration;
	
protected:
	
	
};
