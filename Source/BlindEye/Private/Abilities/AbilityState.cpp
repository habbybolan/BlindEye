// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityState.h"
#include "Abilities/AbilityBase.h"
#include "Abilities/AbilityManager.h"

FAbilityState::FAbilityState(AAbilityBase* ability)
{
	Ability = ability;
}

void FAbilityState::ExitState()
{
	CurrInnerState = EInnerState::Exit;
}

void FAbilityState::RunState(EAbilityInputTypes abilityUsageType)
{
	CurrInnerState = EInnerState::Running;
	if (Ability)
	{
		Ability->AbilityEnteredRunState.ExecuteIfBound(Ability);
	}
}

void FAbilityState::CancelState()
{
	ResetState();
}

void FAbilityState::HandleInput(EAbilityInputTypes abilityUsageType)
{ 
	if (CurrInnerState == EInnerState::None)
	{
		TryEnterState(abilityUsageType);
	} else if (CurrInnerState == EInnerState::Running)
	{
		RunState(abilityUsageType);
	}
}

void FAbilityState::ResetState()
{
	CurrInnerState = EInnerState::None;
}

