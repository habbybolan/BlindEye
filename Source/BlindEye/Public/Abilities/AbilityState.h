// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BLINDEYE_API AbilityState 
{
public:
	AbilityState();

	/**
	 * @return True if the enter state conditions were satisfied
	 */
	virtual bool EnterState() = 0;
	virtual void ExitState() = 0;
	virtual void RunState() = 0;
};
