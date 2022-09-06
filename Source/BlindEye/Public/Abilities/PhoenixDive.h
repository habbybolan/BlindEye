// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "PhoenixDive.generated.h"

// Jumping State
class BLINDEYE_API FJumpState : public FAbilityState
{
public:
	FJumpState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};
 
// Extended in air state and launching to ground
class BLINDEYE_API FInAirState : public FAbilityState
{
public:
	FInAirState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API APhoenixDive : public AAbilityBase 
{
	GENERATED_BODY()

public:
	APhoenixDive();

	// launches character upwards and sets transition
	void LaunchPlayerUpwards();
	void HangInAir();
	void HangInAirTimer();
	void LaunchToGround();
	
	FTimerHandle HangInAirTimerHandle;

protected:

	void EndLaunchUp();
	
};
