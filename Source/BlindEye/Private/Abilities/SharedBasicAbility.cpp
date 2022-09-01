// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

// First Attack state

ASharedBasicAbility::ASharedBasicAbility()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));
}

UFirstAttackState::UFirstAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

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

USecondAttackState::USecondAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

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

ULastAttackState::ULastAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

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
