// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRush.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ACrowRush::ACrowRush()
{
	AbilityStates.Add(new FCrowRushStartState(this));
}

void ACrowRush::UpdatePlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600 * DashSpeedIncrease;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048 * DashAccelerationIncrease;
	}
}

void ACrowRush::ResetPlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048;
	}
}

// **** States *******

// Dash Start State *********************

FCrowRushStartState::FCrowRushStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FCrowRushStartState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FCrowRushStartState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (Ability == nullptr) return;
	ACrowRush* Dash = Cast<ACrowRush>(Ability);
	if (Dash == nullptr) return;

	Dash->UpdatePlayerSpeed();
	Dash->DelayToNextState(Dash->DashDuration, true);
}

void FCrowRushStartState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;

	ACrowRush* Dash = Cast<ACrowRush>(Ability);
	if (Dash == nullptr) return;

	Dash->ResetPlayerSpeed();
	Ability->EndCurrState();
}
