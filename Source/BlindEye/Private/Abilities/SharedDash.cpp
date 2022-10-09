// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedDash.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASharedDash::ASharedDash()
{
	AbilityStates.Add(new FDashStartState(this));
}

void ASharedDash::UpdatePlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600 * DashSpeedIncrease;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048 * DashAccelerationIncrease;
	}
}

void ASharedDash::ResetPlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048;
	}
}

// **** States *******

// Dash Start State *********************

FDashStartState::FDashStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FDashStartState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FDashStartState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (Ability == nullptr) return;
	ASharedDash* Dash = Cast<ASharedDash>(Ability);
	if (Dash == nullptr) return;

	Dash->UpdatePlayerSpeed();
	Dash->DelayToNextState(Dash->DashDuration, true);
}

void FDashStartState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;

	ASharedDash* Dash = Cast<ASharedDash>(Ability);
	if (Dash == nullptr) return;

	Dash->ResetPlayerSpeed();
	Ability->EndCurrState();
}
