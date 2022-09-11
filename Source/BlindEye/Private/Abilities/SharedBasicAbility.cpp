// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

ASharedBasicAbility::ASharedBasicAbility() : AAbilityBase()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));
}

void ASharedBasicAbility::TryCancelAbility()
{
	Super::TryCancelAbility();
}

void ASharedBasicAbility::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	ClearLeaveAbilityTimer();
}

void ASharedBasicAbility::SpawnFlock_Implementation(uint8 comboNum)
{
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.Instigator = GetInstigator();
	params.Owner = GetInstigator();

	switch (comboNum)
	{
	case 0:
		world->SpawnActor<ABasicAttackSmallFlock>(FirstChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
		break;
	case 1:
		world->SpawnActor<ABasicAttackSmallFlock>(SecondChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
		break;
	case 2:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
		break;
	default:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
		break;
	}
}

void ASharedBasicAbility::SetLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
	world->GetTimerManager().SetTimer(ResetAbilityTimerHandle, this, &AAbilityBase::TryCancelAbility, AbilityCancelDelay, false);
} 

void ASharedBasicAbility::ClearLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
}

// **** States *******

// First Attack state *********************

UFirstAttackState::UFirstAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

void UFirstAttackState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void UFirstAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(0);
	}
	ExitState();
}

void UFirstAttackState::ExitState()
{
	FAbilityState::ExitState();
	// Exits the ability if no input in time
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SetLeaveAbilityTimer();
		SharedAbility->DelayToNextState(SharedAbility->ChargeDelay1);
	}
}

// Second Attack state *********************

USecondAttackState::USecondAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

void USecondAttackState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void USecondAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(1);
	}
	ExitState();
}

void USecondAttackState::ExitState()
{
	// Exits the ability if no input in time
	FAbilityState::ExitState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SetLeaveAbilityTimer();
		SharedAbility->DelayToNextState(SharedAbility->ChargeDelay2);
	}
}

// Last Attack state *********************

ULastAttackState::ULastAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

void ULastAttackState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void ULastAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(2);
	}
	ExitState();
}

void ULastAttackState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}
