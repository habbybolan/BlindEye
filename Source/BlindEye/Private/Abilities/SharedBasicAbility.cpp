// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

#include "Characters/BlindEyePlayerCharacter.h"

ASharedBasicAbility::ASharedBasicAbility() : AAbilityBase()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));

	ChargeAnimMontages.SetNum(3);
}

void ASharedBasicAbility::WaitForUseAbilityNotify(uint8 Charge)
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_PlayAnimMontage(ChargeAnimMontages[Charge]);
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void ASharedBasicAbility::UseAnimNotifyExecuted()
{
	SpawnFlock(0);
	AnimNotifyDelegate.Unbind();
	WaitForEndAbilityNotify();
}

void ASharedBasicAbility::WaitForEndAbilityNotify()
{
	AnimNotifyDelegate.BindUFunction( this, TEXT("EndAnimNotifyExecuted"));
}

void ASharedBasicAbility::EndAnimNotifyExecuted()
{
	AbilityStates[CurrState]->ExitState();
	AnimNotifyDelegate.Unbind();
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
	if (!Ability) return;
	if (ASharedBasicAbility* SharedBasicAbility = Cast<ASharedBasicAbility>(Ability))
	{
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->FirstChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void UFirstAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();

	if (ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability))
	{
		SharedAbility->WaitForUseAbilityNotify(0);
	}
	
	if (!Ability) return;
	Ability->StartLockRotation(0);
	Ability->BP_AbilityStarted();
	Ability->BP_AbilityInnerState(1);
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
	if (!Ability) return;
	Ability->StartLockRotation(0);
	if (ASharedBasicAbility* SharedBasicAbility = Cast<ASharedBasicAbility>(Ability))
	{
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->SecondChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void USecondAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	if (!Ability) return;
	Ability->StartLockRotation(0);
	Ability->BP_AbilityInnerState(2);
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
	if (!Ability) return;
	if (ASharedBasicAbility* SharedBasicAbility = Cast<ASharedBasicAbility>(Ability))
	{
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->ThirdChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void ULastAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	if (!Ability) return;
	Ability->BP_AbilityInnerState(3);
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
