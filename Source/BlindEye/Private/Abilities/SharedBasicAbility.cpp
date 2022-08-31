// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

// First Attack state

USharedBasicAbility::USharedBasicAbility()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));
}

UFirstAttackState::UFirstAttackState(UAbilityBase* ability) : FAbilityState(ability) {}

bool UFirstAttackState::TryEnterState(bool bInputUsed)
{
	// TODO: No Condition to enter (Enters immediately on ability use)
	return true;
}

void UFirstAttackState::RunState()
{
	
	// TODO:
}

void UFirstAttackState::ExitState()
{
	// TODO: Timer that exits the Ability combo
	FAbilityState::ExitState();
}

// Second Attack state

USecondAttackState::USecondAttackState(UAbilityBase* ability) : FAbilityState(ability) {}

bool USecondAttackState::TryEnterState(bool bInputUsed)
{
	// TODO: Input Condition ToEnter
	return true;
}

void USecondAttackState::RunState()
{
	// TODO:
}

void USecondAttackState::ExitState()
{
	// TODO: Timer that exits the Ability combo
	FAbilityState::ExitState();
}

// Last Attack state

ULastAttackState::ULastAttackState(UAbilityBase* ability) : FAbilityState(ability) {}

bool ULastAttackState::TryEnterState(bool bInputUsed)
{
	// TODO: Input Condition ToEnter
	return true;
}

void ULastAttackState::RunState()
{
	// TODO:
}

void ULastAttackState::ExitState()
{
	// TODO: Timer that exits the Ability combo
	FAbilityState::ExitState();
}
