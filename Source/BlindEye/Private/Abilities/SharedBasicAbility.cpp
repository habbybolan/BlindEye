// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

#include "Characters/BlindEyePlayerCharacter.h"

ASharedBasicAbility::ASharedBasicAbility() : AAbilityBase()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));

	ChargeAnimMontages.SetNum(3);
	AbilityType = EAbilityTypes::Basic;
}

void ASharedBasicAbility::SetComboFinished()
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
	Player->TryFinishTutorial(ETutorialChecklist::Combo);
}

void ASharedBasicAbility::PlayAbilityAnimation()
{
	bIsAttacking = true;
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_PlayAnimMontage(ChargeAnimMontages[CurrCharge]);
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void ASharedBasicAbility::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	SpawnFlock();
	WaitForEndAbilityNotify();
}

void ASharedBasicAbility::WaitForEndAbilityNotify()
{
	AnimNotifyDelegate.BindUFunction( this, TEXT("EndAnimNotifyExecuted"));
}

void ASharedBasicAbility::EndAnimNotifyExecuted()
{
	bIsAttacking = false;
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void ASharedBasicAbility::TryCancelAbility()
{
	Super::TryCancelAbility();
	AnimNotifyDelegate.Unbind();
	bIsAttacking = false;
}

void ASharedBasicAbility::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	ClearLeaveAbilityTimer();
	CurrCharge = 0;
}

void ASharedBasicAbility::SpawnFlock_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.Instigator = GetInstigator();
	params.Owner = GetInstigator();
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	switch (CurrCharge)
	{
	case 0:
		world->SpawnActor<ABasicAttackSmallFlock>(FirstChargeFlockType, FVector::ZeroVector, FRotator::ZeroRotator, params);
		break;
	case 1:
		world->SpawnActor<ABasicAttackSmallFlock>(SecondChargeFlockType, FVector::ZeroVector, FRotator::ZeroRotator, params);
		break;
	case 2:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, FVector::ZeroVector, FRotator::ZeroRotator, params);
		break;
	default:
		world->SpawnActor<ABasicAttackSmallFlock>(LastChargeFlockType, FVector::ZeroVector, FRotator::ZeroRotator, params);
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
		SharedBasicAbility->CurrCharge = 0;
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
		// prevent re-entering state while attacking
		if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

		SharedAbility->Blockers.IsMovementSlowBlocked = true;
		SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge1MovementSlow;
		SharedAbility->PlayAbilityAnimation();
	}
	
	if (!Ability) return;
	Ability->StartLockRotation(1);
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
	if (ASharedBasicAbility* SharedBasicAbility = Cast<ASharedBasicAbility>(Ability))
	{
		// set current charge
		SharedBasicAbility->CurrCharge = 1;
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->SecondChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void USecondAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	if (!Ability) return;
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (!SharedAbility) return;

	// prevent re-entering state while attacking
	if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

	SharedAbility->Blockers.IsMovementSlowBlocked = true;
	SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge2MovementSlow;
	SharedAbility->ClearLeaveAbilityTimer();
	SharedAbility->StartLockRotation(1);
	SharedAbility->BP_AbilityInnerState(2);
	SharedAbility->PlayAbilityAnimation();
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
		// set current charge
		SharedBasicAbility->CurrCharge = 2;
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->ThirdChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void ULastAttackState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	if (!Ability) return;
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility == nullptr) return;

	// prevent re-entering state while attacking
	if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

	SharedAbility->Blockers.IsMovementSlowBlocked = true;
	SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge3MovementSlow;
	SharedAbility->ClearLeaveAbilityTimer();
	SharedAbility->StartLockRotation(2);
	SharedAbility->BP_AbilityInnerState(3);
	SharedAbility->PlayAbilityAnimation();
	SharedAbility->SetComboFinished();
}

void ULastAttackState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}
