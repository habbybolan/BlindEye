// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowCocoon.h"

ACrowCocoon::ACrowCocoon() : AAbilityBase()
{
	AbilityStates.Add(new UCrowCocoonStart(this));
	AbilityStates.Add(new UCrowCocoonStart(this));
}

// **** States *******

// Cocoon Start State *********************

UCrowCocoonStart::UCrowCocoonStart(AAbilityBase* ability): FAbilityState(ability) {}

void UCrowCocoonStart::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
}

void UCrowCocoonStart::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
}

void UCrowCocoonStart::ExitState()
{
	FAbilityState::ExitState();
}

// Cocoon Pulse State *********************

UCrowPulseState::UCrowPulseState(AAbilityBase* ability) : FAbilityState(ability) {}

void UCrowPulseState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
}

void UCrowPulseState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
}

void UCrowPulseState::ExitState()
{
	FAbilityState::ExitState();
}


