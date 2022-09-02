// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityState.h"
#include "Abilities/AbilityBase.h"

FAbilityState::FAbilityState(AAbilityBase* ability)
{
	Ability = ability;
}

void FAbilityState::ExitState()
{
}

void FAbilityState::RunState()
{
	bStateEntered = true;
}

void FAbilityState::CancelState()
{
	ResetState();
}

void FAbilityState::ResetState()
{
	bStateEntered = false;
}

