// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixDive.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

APhoenixDive::APhoenixDive() : AAbilityBase()
{
	AbilityStates.Add(new FJumpState(this));
	AbilityStates.Add(new FInAirState(this));
}

void APhoenixDive::LaunchPlayerUpwards()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * 100000);
}

void APhoenixDive::HangInAir()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 0.f;
	Character->GetCharacterMovement()->StopMovementImmediately();
}

void APhoenixDive::HangInAirTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(HangInAirTimerHandle, this, &APhoenixDive::HangInAir, 1.0f, false);
}

void APhoenixDive::LaunchToGround()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 1.f;

	FRotator LaunchRotation = Character->GetControlRotation();
	Character->GetCharacterMovement()->AddImpulse(LaunchRotation.Vector() * 200000);

	UWorld* world = GetWorld();
	if (!world) return;

	// prevent hanging in air
	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
}

void APhoenixDive::EndLaunchUp()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	EndCurrState();
	// immediately enter new state
	UseAbility(EAbilityInputTypes::None);
}

// **** States *******

// Jumping State *********************

FJumpState::FJumpState(AAbilityBase* ability) : FAbilityState(ability) {}

void FJumpState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FJumpState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (!Ability) return;
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	PhoenixDive->LaunchPlayerUpwards();
	ExitState();
}

void FJumpState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;

	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

// In Air State *********************

FInAirState::FInAirState(AAbilityBase* ability) : FAbilityState(ability) {}

void FInAirState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FInAirState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	
	if (!Ability) return;
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;
	
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		PhoenixDive->LaunchToGround();
		ExitState();
	} else
	{
		PhoenixDive->HangInAirTimer();
	}
}

void FInAirState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
}

