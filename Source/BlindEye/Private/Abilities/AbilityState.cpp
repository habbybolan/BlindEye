// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityState.h"
#include "Abilities/AbilityBase.h"

FAbilityState::FAbilityState(UAbilityBase* ability)
{
	Ability = ability;
}

void FAbilityState::ExitState()
{
	ResetState();
	Ability->EndCurrState();
}

void FAbilityState::ResetState()
{
	bStateEntered = false;
}

