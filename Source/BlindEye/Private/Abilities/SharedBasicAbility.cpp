// Copyright (C) Nicholas Johnson 2022


#include "Abilities/SharedBasicAbility.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	//Player->CLI_TryFinishTutorial(ETutorialChecklist::Combo);
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
	SER_SpawnFlock();
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

FVector ASharedBasicAbility::CalcFirstFlockingTarget()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return FVector::ZeroVector;
		
	FVector ViewportLocation;
	FRotator ViewportRotation;
	GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);
	FVector InstigatorFwd =  ViewportRotation.Vector() * TargetDistanceFromInstigator;

	FVector TargetLocation;
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(World, ViewportLocation, ViewportLocation + InstigatorFwd, SpawnLineCastObjectTypes,
		false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
	{
		TargetLocation = OutHit.Location;
	} else
	{
		TargetLocation = ViewportLocation + InstigatorFwd;
	}
	return TargetLocation;
}

void ASharedBasicAbility::SER_SpawnFlock_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FName BoneSpawnLocation;
	TSubclassOf<ABasicAttackSmallFlock> FlockType;
	switch (CurrCharge)
	{
	case 0:
		BoneSpawnLocation = FName("RightHand");
		FlockType = FirstChargeFlockType;
		break;
	case 1:
		BoneSpawnLocation = FName("LeftHand");
		FlockType = SecondChargeFlockType;
		break;
	case 2:
		BoneSpawnLocation = FName("RightHand");
		FlockType = LastChargeFlockType;
		break;
	default:
		BoneSpawnLocation = FName("RightHand");
		FlockType = FirstChargeFlockType;
		break;
	}
	MULT_SpawnFlockHelper(BoneSpawnLocation, FlockType, CalcFirstFlockingTarget());
}

void ASharedBasicAbility::MULT_SpawnFlockHelper_Implementation(FName BoneSpawnLocation,
	TSubclassOf<ABasicAttackSmallFlock> FlockType, FVector StartTargetLoc)
{
	UWorld* World = GetWorld();
	if (World) 
	{
		ABasicAttackSmallFlock* Flock = World->SpawnActorDeferred<ABasicAttackSmallFlock>(FlockType, FTransform::Identity, GetOwner(),
			GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Flock->BoneSpawnLocation = BoneSpawnLocation;
		Flock->InitialTarget = StartTargetLoc;
		UGameplayStatics::FinishSpawningActor(Flock, FTransform::Identity);
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
	Ability->AbilityStarted();
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
