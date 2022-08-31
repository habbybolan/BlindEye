// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"

class UAbilityBase;
/**
 * 
 */
class BLINDEYE_API FAbilityState 
{
public:
	FAbilityState(UAbilityBase* ability);

	/**
	 * @return True if the enter state conditions were satisfied
	 */
	virtual bool TryEnterState(bool bInputUsed = false) = 0;
	virtual void ExitState();
	virtual void RunState() = 0;

	// reset the state after leaving it for entering again in the future
	virtual void ResetState();

	// False for states not yet reached or has attempted and failed to pass entry state
	bool bStateEntered = false;
	bool bEntersWithInput = true;

protected:
	UAbilityBase* Ability;
};
