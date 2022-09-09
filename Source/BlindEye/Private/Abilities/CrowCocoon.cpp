// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowCocoon.h"

ACrowCocoon::ACrowCocoon() : AAbilityBase()
{
	AbilityStates.Add(new UCrowCocoonStart(this));
	AbilityStates.Add(new UCrowPulseState(this));
}

void ACrowCocoon::StartHoldLogic()
{
	TimeHoldStart = GetGameTimeSinceCreation();
}

void ACrowCocoon::EndHold()
{
	float CurrTime = GetGameTimeSinceCreation();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Emerald, "Held for: " + FString::SanitizeFloat(CurrTime - TimeHoldStart));
}

// **** States *******

// Cocoon Start State *********************

UCrowCocoonStart::UCrowCocoonStart(AAbilityBase* ability): FAbilityState(ability) {}

void UCrowCocoonStart::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// only enter ability on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void UCrowCocoonStart::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		ExitState();
	} else
	{
		// perform holding logic
		if (!Ability) return;
		if (ACrowCocoon* Cocoon = Cast<ACrowCocoon>(Ability))
		{
			Cocoon->StartHoldLogic();
		}
	}
}

void UCrowCocoonStart::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Cocoon Pulse State *********************

UCrowPulseState::UCrowPulseState(AAbilityBase* ability) : FAbilityState(ability) {}

void UCrowPulseState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void UCrowPulseState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	// Perform explosion attack
	if (!Ability) return;
	if (ACrowCocoon* Cocoon = Cast<ACrowCocoon>(Ability))
	{
		Cocoon->EndHold();
	}
	ExitState();
}

void UCrowPulseState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
}


