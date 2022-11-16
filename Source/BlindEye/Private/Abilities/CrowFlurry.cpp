// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowFlurry.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ACrowFlurry::ACrowFlurry()
{
	AbilityStates.Add(new FPerformCrowFlurryState(this));
	AbilityStates.Add(new FCancelCrowFlurryState(this));
	AbilityStates.Add(new FEndCrowFlurryState(this));
	AbilityType = EAbilityTypes::Unique2;
}

void ACrowFlurry::BeginPlay()
{
	Super::BeginPlay();
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	BP_AbilityInnerState(1);

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayerCharacter->CLI_UpdateRotationRate(RotationSpeedInFlurry);
	}

	bFlurryActive = true;
	CurrFlurryRotation = GetInstigator()->GetControlRotation();
	world->GetTimerManager().SetTimer(CrowFlurryTimerHandle, this, &ACrowFlurry::PerformCrowFlurry, CrowFlurryDamageDelay, true);
	// Lerp the flurry rotation towards controller rotation
	world->GetTimerManager().SetTimer(CalculateRotationTimerHandle, this, &ACrowFlurry::CalcFlurryRotation, CalcRotationDelay, true);
	// flurry rotation separate for replication reasons
	world->GetTimerManager().SetTimer(RotateFlurryTimerHandle, this, &ACrowFlurry::MULT_RotateFlurry, CalcRotationDelay, true);
	// Max Duration of flurry
	world->GetTimerManager().SetTimer(DurationTimerHandle, this, &ACrowFlurry::CrowFlurryDurationEnd, Duration, false);
}

void ACrowFlurry::PlayAbilityAnimation()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_StartLockRotationToController(1);
		PlayerCharacter->MULT_PlayAnimMontage(CrowFlurryAnimation);
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void ACrowFlurry::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	StartCrowFlurry();
	AbilityStates[CurrState]->ExitState();
}

void ACrowFlurry::EndAbilityAnimation()
{
	BP_AbilityInnerState(2);
	
	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
	world->GetTimerManager().ClearTimer(CalculateRotationTimerHandle);
	world->GetTimerManager().ClearTimer(DurationTimerHandle);
	
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_SetNextMontageSection(CrowFlurryAnimation, "End");
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("AbilityAnimationEnded"));
}

void ACrowFlurry::AbilityAnimationEnded()
{
	AbilityStates[CurrState]->ExitState();
}

void ACrowFlurry::MULT_RotateFlurry_Implementation()
{
	RotateFlurryHelper();
}

void ACrowFlurry::CrowFlurryDurationEnd()
{
	// manually input to stop crow flurry
	AbilityStates[CurrState]->HandleInput(EAbilityInputTypes::Pressed);
}

void ACrowFlurry::PerformCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;
 
	FVector InstigatorFwd =  CurrFlurryRotation.Vector() * Range;
	FVector TargetLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), TargetLocation, FVector(0, Width / 2, Height / 2),
		GetInstigator()->GetControlRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	
	for (FHitResult Hit : OutHits)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamagePerSec * CrowFlurryDamageDelay, FVector::ZeroVector,
					FHitResult(), GetInstigator()->Controller, this, DamageType);
	}

	// consume bird meter
	if (!TryConsumeBirdMeter(CostPercentPerSec * 0.2f))
	{
		// if out of bird meter, simulate releasing button
		AbilityStates[CurrState]->HandleInput(EAbilityInputTypes::Pressed);
	}

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayerCharacter->MULT_StartLockRotationToController(0);
	}
} 

void ACrowFlurry::CalcFlurryRotation()
{
	FRotator TargetRotation = GetInstigator()->GetControlRotation();
	CurrFlurryRotation = UKismetMathLibrary::RLerp(CurrFlurryRotation, TargetRotation, CrowFlurryLerpSpeed, true);
}

void ACrowFlurry::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	bFlurryActive = false;

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayerCharacter->CLI_ResetRotationRateToNormal();
	}
}

// **** States *******

// Animation to start crow flurry state  *********************

FPerformCrowFlurryState::FPerformCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

void FPerformCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (abilityUsageType != EAbilityInputTypes::Pressed) return;
	
	// apply initial cost
	if (!Ability) return;
	Ability->SetOnCooldown();
	if (ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability))
	{
		if (!CrowFlurry->TryConsumeBirdMeter(CrowFlurry->InitialCostPercent)) return;
	}
	RunState();
}

void FPerformCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	// Dont enter if input pressed
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState();
	if (!Ability) return;
	
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (!CrowFlurry) return;
	
	Ability->AbilityStarted();
	// wait for ability startup to goto end state
	CrowFlurry->PlayAbilityAnimation();

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
}

void FPerformCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

void FPerformCrowFlurryState::CancelState()
{
	FAbilityState::CancelState();

	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(CrowFlurry->GetOwner());
	Player->CLI_StopAnimation(0, CrowFlurry->CrowFlurryAnimation);
}

// State to Start actually using the crow flurry *****************

FCancelCrowFlurryState::FCancelCrowFlurryState(AAbilityBase* ability)  : FAbilityState(ability) {}

void FCancelCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FCancelCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{ 
	FAbilityState::RunState(abilityUsageType);

	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (CrowFlurry == nullptr) return;

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		CrowFlurry->EndAbilityAnimation();
		ExitState();
	}
}

void FCancelCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

void FCancelCrowFlurryState::CancelState()
{
	FAbilityState::CancelState();
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(CrowFlurry->GetOwner());
	Player->CLI_StopAnimation(0, CrowFlurry->CrowFlurryAnimation);

	UWorld* World = Ability->GetWorld();
	if(World)
	{
		World->GetTimerManager().ClearTimer(CrowFlurry->CrowFlurryTimerHandle);
		World->GetTimerManager().ClearTimer(CrowFlurry->CalculateRotationTimerHandle);
		World->GetTimerManager().ClearTimer(CrowFlurry->RotateFlurryTimerHandle);
		World->GetTimerManager().ClearTimer(CrowFlurry->DurationTimerHandle);
	}
	
}

// State to wait for animation to end after crow flurry cancelled / stopped

FEndCrowFlurryState::FEndCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

void FEndCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FEndCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{ 
	FAbilityState::RunState(abilityUsageType);
	Ability->Blockers.IsMovementSlowBlocked = true;
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	Ability->Blockers.MovementSlowAmount = CrowFlurry->MovementSlowAfterStoppingFlurry;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	Ability->TryCancelAbility();
}

void FEndCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

void FEndCrowFlurryState::CancelState()
{
	FAbilityState::CancelState();

	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(CrowFlurry->GetOwner());
	Player->CLI_StopAnimation(0, CrowFlurry->CrowFlurryAnimation);
}


void ACrowFlurry::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrowFlurry, CurrFlurryRotation);
}


