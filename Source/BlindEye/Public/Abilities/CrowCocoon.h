// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "CrowCocoon.generated.h"

class BLINDEYE_API UCrowCocoonStart : public FAbilityState
{ 
public:  
	UCrowCocoonStart(AAbilityBase* ability);
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
	TArray<float> DamageTicks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BirdMeterPercentLossPerSec = 20; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHoldDuration = 4.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayFirstPulse = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Radius = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float CostPercentPerSec = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float InitialCostPercent = 20; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> MainDamageType;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TSubclassOf<UBaseDamageType> FirstPulseDamageType; 

	void StartHoldLogic();
	virtual void EndAbilityLogic() override;
	
	
protected:

	FTimerHandle PulseTimerHandle;
	float TimeHoldStart;
	const uint8 MaxNumberPulses = 4;

	bool bReachedFinalPulse = false;

	void PerformPulse();

	uint8 CalcPulseIndex();
	
};
