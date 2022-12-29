// Copyright (C) Nicholas Johnson 2022

#include "Abilities/SharedBasicAbility.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ASharedBasicAbility::ASharedBasicAbility() : AAbilityBase()
{
	AbilityStates.Add(new UFirstAttackState(this));
	AbilityStates.Add(new USecondAttackState(this));
	AbilityStates.Add(new ULastAttackState(this));

	ChargeAnimMontages.SetNum(3);
	AbilityType = EAbilityTypes::Basic;
}

void ASharedBasicAbility::AbilityStarted()
{
	Super::AbilityStarted();

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
		Player->TutorialActionPerformed(TutorialInputActions::BasicAttack);
	}
}

void ASharedBasicAbility::SetComboFinished()
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
		Player->TutorialActionPerformed(TutorialInputActions::BasicAttackCombo);
	}
}

void ASharedBasicAbility::PlayAbilityAnimation()
{
	bIsAttacking = true;
	PlayAbilityAnimationHelper(CurrCharge);
	MULT_PlayAbilityAnimation(CurrCharge);

	AnimNotifyDelegate.BindDynamic( this, &ASharedBasicAbility::UseAnimNotifyExecuted);
}

void ASharedBasicAbility::PlayAbilityAnimationHelper(uint8 charge)
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->PlayAnimMontage(ChargeAnimMontages[charge]);
	}
}

void ASharedBasicAbility::MULT_PlayAbilityAnimation_Implementation(uint8 charge)
{
	// Play for remote clients
	if (!GetIsLocallyControlled() && GetInstigator()->GetLocalRole() != ROLE_Authority)
	{
		PlayAbilityAnimationHelper(charge);
	}
}

void ASharedBasicAbility::UseAnimNotifyExecuted()
{
	//AnimNotifyDelegate.Unbind();
	SpawnFlock();
	WaitForEndAbilityNotify();
}

void ASharedBasicAbility::WaitForEndAbilityNotify()
{
	AnimNotifyDelegate.BindDynamic( this, &ASharedBasicAbility::EndAnimNotifyExecuted);
}

void ASharedBasicAbility::EndAnimNotifyExecuted()
{
	bIsAttacking = false;
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

bool ASharedBasicAbility::TryCancelAbility()
{
	Super::TryCancelAbility();
	AnimNotifyDelegate.Unbind();
	bIsAttacking = false;
	return true;
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

	// If Topdown
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	if (Player->GetIsTopdown())
	{
		FVector TargetLoc;
		ABlindEyePlayerController::GetMouseAimLocationHelper(OUT TargetLoc, AimLocation, AimRotation, Player, World);
		return TargetLoc;
	}
	// 3rd person
	else
	{
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
}

void ASharedBasicAbility::TryCancelAbilityHelper()
{
	TryCancelAbility();
}

void ASharedBasicAbility::SpawnFlock()
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
	SpawnFlockHelper(BoneSpawnLocation, FlockType, CalcFirstFlockingTarget());
	MULT_SpawnFlockHelper(BoneSpawnLocation, FlockType, CalcFirstFlockingTarget());
}

void ASharedBasicAbility::SpawnFlockHelper(FName BoneSpawnLocation, TSubclassOf<ABasicAttackSmallFlock> FlockType,
	FVector StartTargetLoc)
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

void ASharedBasicAbility::MULT_SpawnFlockHelper_Implementation(FName BoneSpawnLocation,
	TSubclassOf<ABasicAttackSmallFlock> FlockType, FVector StartTargetLoc)
{
	// Run for remote clients
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		SpawnFlockHelper(BoneSpawnLocation, FlockType, StartTargetLoc);
	}
}

void ASharedBasicAbility::SetLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
	world->GetTimerManager().SetTimer(ResetAbilityTimerHandle, this, &ASharedBasicAbility::TryCancelAbilityHelper, AbilityCancelDelay, false);
} 

void ASharedBasicAbility::ClearLeaveAbilityTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(ResetAbilityTimerHandle);
}

void ASharedBasicAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// **** States *******

// First Attack state *********************

UFirstAttackState::UFirstAttackState(AAbilityBase* ability) : FAbilityState(ability) {}

void UFirstAttackState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	if (ASharedBasicAbility* SharedBasicAbility = Cast<ASharedBasicAbility>(Ability))
	{
		SharedBasicAbility->CurrCharge = 0;
		if (!SharedBasicAbility->TryConsumeBirdMeter(SharedBasicAbility->FirstChargeCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void UFirstAttackState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	if (ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability))
	{
		// prevent re-entering state while attacking
		if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

		SharedAbility->Blockers.IsMovementSlowBlocked = true;
		SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge1MovementSlow;
		SharedAbility->PlayAbilityAnimation();
	} 
	
	Ability->StartLockRotation(1);
	Ability->AbilityStarted();
	Ability->AbilityInnerState(1);
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

void USecondAttackState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
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

void USecondAttackState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (!SharedAbility) return;

	// prevent re-entering state while attacking
	if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

	SharedAbility->Blockers.IsMovementSlowBlocked = true;
	SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge2MovementSlow;
	SharedAbility->ClearLeaveAbilityTimer();
	SharedAbility->StartLockRotation(1);
	SharedAbility->AbilityInnerState(2);
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

void ULastAttackState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
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

void ULastAttackState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	ASharedBasicAbility* SharedAbility = Cast<ASharedBasicAbility>(Ability);
	if (SharedAbility == nullptr) return;

	// prevent re-entering state while attacking
	if (SharedAbility->bIsAttacking || abilityUsageType > EAbilityInputTypes::None) return;

	SharedAbility->Blockers.IsMovementSlowBlocked = true;
	SharedAbility->Blockers.MovementSlowAmount = SharedAbility->Charge3MovementSlow;
	SharedAbility->ClearLeaveAbilityTimer();
	SharedAbility->StartLockRotation(2);
	SharedAbility->AbilityInnerState(3);
	SharedAbility->PlayAbilityAnimation();
	SharedAbility->SetComboFinished();
}

void ULastAttackState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}
