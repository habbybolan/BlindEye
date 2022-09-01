// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

void ASharedBasicAbility::TryCancelAbility()
{
	ClearLeaveAbilityTimer();
	Super::TryCancelAbility();
}

void ASharedBasicAbility::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	UWorld* world = GetWorld();
	if (world)
	{
		world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
	}
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
	case 1:
		world->SpawnActor<ABasicAttackSmallFlock>(SecondChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
	case 2:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
	default:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
	}
}

// **** States *******

// First Attack state

ASharedBasicAbility::ASharedBasicAbility() : AAbilityBase()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));
}

void ASharedBasicAbility::SetLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(ResetAbilityTimerHandle, this, &AAbilityBase::TryCancelAbility, 2, false);
}

void ASharedBasicAbility::ClearLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
}

UFirstAttackState::UFirstAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

bool UFirstAttackState::TryEnterState(bool bInputUsed)
{
	if (bStateEntered) return false;
	RunState();
	return true;
}

void UFirstAttackState::RunState()
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(0);
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: 1");
	ExitState();
}

void UFirstAttackState::ExitState()
{
	// Exits the ability if no input in time
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SetLeaveAbilityTimer();
		SharedAbility->DelayToNextState(.2f);
	}
	
	FAbilityState::ExitState();
}

// Second Attack state

USecondAttackState::USecondAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

bool USecondAttackState::TryEnterState(bool bInputUsed)
{
	if (bStateEntered) return false;
	// TODO: Input Condition ToEnter
	RunState();
	return true;
}

void USecondAttackState::RunState()
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(1);
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: 2");
	ExitState();
}

void USecondAttackState::ExitState()
{
	// Exits the ability if no input in time
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SetLeaveAbilityTimer();
		SharedAbility->DelayToNextState(.2f);
	}
	
	FAbilityState::ExitState();
	
}

// Last Attack state

ULastAttackState::ULastAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

bool ULastAttackState::TryEnterState(bool bInputUsed)
{
	if (bStateEntered) return false;
	// TODO: Input Condition ToEnter
	RunState();
	return true;
}

void ULastAttackState::RunState()
{
	FAbilityState::RunState();
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility)
	{
		SharedAbility->SpawnFlock(2);
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Charge: last");
	ExitState();
}

void ULastAttackState::ExitState()
{
	Ability->EndCurrState();
	FAbilityState::ExitState();
}
