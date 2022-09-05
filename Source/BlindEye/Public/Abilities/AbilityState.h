// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityManager.h"

enum class EAbilityInputTypes;

UENUM()
enum class EInnerState
{
	None,
	Running,	// Running continuous logic
	Exit		// Waiting on transition logic to goto next state
};  

class AAbilityBase;
/**
 * 
 */
class BLINDEYE_API FAbilityState 
{
public:
	FAbilityState(AAbilityBase* ability);

	/** @return True if the enter state conditions were satisfied */
	// State checks for condition and goes to running state if passed. Called from either an event or Input
	virtual bool TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) = 0;

	// Single use logic to start ability logic, or timer that continuously calls Run State logic until run state left
	// Leaving run state either on a timer based, outside event or Input (ex) Bird meter, Input, Status effect)
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None);
	
	// calls any transition logic that must occur before going to next state
	// Transition logic is never input based
	virtual void ExitState();
	
	// Children can add functionality for cleaning up the state
	virtual void CancelState();

	virtual void HandleInput(EAbilityInputTypes abilityUsageType);

	// reset the state after leaving it for entering again in the future
	virtual void ResetState();
 
	// False for states not yet reached or has attempted and failed to pass entry state
	EInnerState CurrInnerState = EInnerState::None;
	bool bEntersWithInput = true;

protected:
	AAbilityBase* Ability;
};
