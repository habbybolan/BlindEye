// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityState.h"
#include "Abilities/AbilityBase.h"
#include "Abilities/AbilityManager.h"

FAbilityState::FAbilityState(AAbilityBase* ability)
{
	Ability = ability;
}

void FAbilityState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	check(Ability);
	RemoveBlockers();
}

void FAbilityState::ExitState()
{
	check(Ability);
	CurrInnerState = EInnerState::Exit;
	RemoveBlockers();
}

void FAbilityState::RunState(EAbilityInputTypes abilityUsageType)
{
	check(Ability);
	CurrInnerState = EInnerState::Running;
	RemoveBlockers();
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
	RemoveBlockers();
}

void FAbilityState::RemoveBlockers()
{
	if (!Ability) return;
	Ability->Blockers.Reset();
}

