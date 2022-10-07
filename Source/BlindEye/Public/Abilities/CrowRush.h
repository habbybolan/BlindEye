// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
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

	void UpdatePlayerSpeed(); 
	void ResetPlayerSpeed();
	
};
