// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityState.h"
#include "Abilities/AbilityBase.h"
#include "Abilities/AbilityManager.h"

FAbilityState::FAbilityState(AAbilityBase* ability)
{
	Ability = ability;
}

void FAbilityState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	check(Ability);
	if (abilityUsageType != EAbilityInputTypes::None)
	{
		Ability->StoreAimData(Location, Rotation);
	}
	RemoveBlockers();
}

void FAbilityState::ExitState()
{
	check(Ability);
	CurrInnerState = EInnerState::Exit;
	RemoveBlockers();
}

void FAbilityState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	check(Ability);
	if (abilityUsageType != EAbilityInputTypes::None)
	{
		Ability->StoreAimData(Location, Rotation);
	}
	CurrInnerState = EInnerState::Running;
	RemoveBlockers();
	if (Ability)
	{
		Ability->AbilityEnteredRunState.ExecuteIfBound(Ability);
	}
}

bool FAbilityState::CancelState()
{
	ResetState();
	return true;
}

void FAbilityState::HandleInput(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	if (CurrInnerState == EInnerState::None)
	{
		TryEnterState(abilityUsageType, Location, Rotation);
	} else if (CurrInnerState == EInnerState::Running)
	{
		RunState(abilityUsageType, Location, Rotation);
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

