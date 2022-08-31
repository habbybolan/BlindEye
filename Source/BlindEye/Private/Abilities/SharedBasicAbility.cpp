// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

// First Attack state

USharedBasicAbility::USharedBasicAbility()
{
	FirstAttackState = new UFirstAttackState();
	SecondAttackState = new USecondAttackState();
	LastAttackState = new ULastAttackState();
}

UFirstAttackState::UFirstAttackState() {}

bool UFirstAttackState::EnterState()
{
	// TODO: No Condition to enter
	return true;
}

void UFirstAttackState::RunState()
{
	// TODO:
}

void UFirstAttackState::ExitState()
{
	// TODO: Timer that exits the Ability combo
}

// Second Attack state

USecondAttackState::USecondAttackState() {}

bool USecondAttackState::EnterState()
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
}

// Last Attack state

ULastAttackState::ULastAttackState() {}

bool ULastAttackState::EnterState()
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
}
