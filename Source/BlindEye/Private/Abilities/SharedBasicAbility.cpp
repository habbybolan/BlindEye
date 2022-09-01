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
	RunState();
	return true;
}

void UFirstAttackState::RunState()
{
	// TODO:
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: 1");
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
	RunState();
	return true;
}

void USecondAttackState::RunState()
{
	// TODO:
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: 2");
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
	RunState();
	return true;
}

void ULastAttackState::RunState()
{
	// TODO:
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: last");
}

void ULastAttackState::ExitState()
{
	// TODO: Timer that exits the Ability combo
	FAbilityState::ExitState();
}
