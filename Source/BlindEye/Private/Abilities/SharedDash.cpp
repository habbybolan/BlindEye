// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedDash.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASharedDash::ASharedDash()
{
	AbilityStates.Add(new FDashStartState(this));
	AbilityType = EAbilityTypes::Dash;
}

void ASharedDash::AbilityStarted()
{
	Super::AbilityStarted();

	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
	Player->TutorialActionPerformed(TutorialInputActions::Dash);
}

void ASharedDash::UpdatePlayerSpeed()
{
	UpdatePlayerSpeedHelper();
	MULT_UpdatePlayerSpeed();
}

void ASharedDash::UpdatePlayerSpeedHelper()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->UpdateWalkMovementSpeed(DashSpeedIncrease, DashAccelerationIncrease);
	}
}

void ASharedDash::MULT_UpdatePlayerSpeed_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		UpdatePlayerSpeedHelper();
	}
}

void ASharedDash::ResetPlayerSpeed()
{
	ResetPlayerSpeedHelper();
	MULT_ResetPlayerSpeed();
}

void ASharedDash::ResetPlayerSpeedHelper()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->ResetWalkMovementToNormal();
	}
}

void ASharedDash::MULT_ResetPlayerSpeed_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ResetPlayerSpeedHelper();
	}
}

void ASharedDash::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	ResetPlayerSpeed();

	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = false;
		Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}
}

// **** States *******

// Dash Start State *********************

FDashStartState::FDashStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FDashStartState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Ability->GetOwner()))
		{
			Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = false;
			Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
		}
		RunState();
	}
}

void FDashStartState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType);
	if (Ability == nullptr) return;

	Ability->AbilityStarted();
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	ASharedDash* Dash = Cast<ASharedDash>(Ability);
	if (Dash == nullptr) return;

	Dash->UpdatePlayerSpeed();
	Dash->DelayToNextState(Dash->DashDuration, true);
}

void FDashStartState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;
	Ability->EndCurrState();
}

bool FDashStartState::CancelState()
{
	return FAbilityState::CancelState();
}
