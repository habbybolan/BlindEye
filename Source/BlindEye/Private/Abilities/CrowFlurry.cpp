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

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		CachedCharacterRotationSpeed = BlindEyePlayerCharacter->GetCharacterMovement()->RotationRate.Yaw;
	}
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	BP_AbilityInnerState(1);

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayerCharacter->GetCharacterMovement()->RotationRate = FRotator(0, RotationSpeedInFlurry, 0);
	}

	bFlurryActive = true;
	CurrFlurryRotation = GetInstigator()->GetControlRotation();
	world->GetTimerManager().SetTimer(CrowFlurryTimerHandle, this, &ACrowFlurry::PerformCrowFlurry, CrowFlurryDamageDelay, true);
	// Lerp the flurry rotation towards controller rotation
	world->GetTimerManager().SetTimer(CalculateRotationTimerHandle, this, &ACrowFlurry::CalcFlurryRotation, CalcRotationDelay, true);
	// flurry rotation separate for replication reasons
	world->GetTimerManager().SetTimer(RotateFlurryTimerHandle, this, &ACrowFlurry::MULT_RotateFlurry, CalcRotationDelay, true);
}

void ACrowFlurry::PlayAbilityAnimation()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->CLI_StartLockRotationToController(1);
		PlayerCharacter->MULT_PlayAnimMontage(CrowFlurryAnimation);
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void ACrowFlurry::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	StartCrowFlurry();
}

void ACrowFlurry::EndAbilityAnimation()
{
	BP_AbilityInnerState(2);
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
		BlindEyePlayerCharacter->CLI_StartLockRotationToController(0);
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

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
	world->GetTimerManager().ClearTimer(CalculateRotationTimerHandle);

	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayerCharacter->GetCharacterMovement()->RotationRate = FRotator(0, CachedCharacterRotationSpeed, 0);
	}
}

// **** States *******

// First Crow Flurry state *********************

FPerformCrowFlurryState::FPerformCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

void FPerformCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (abilityUsageType != EAbilityInputTypes::Pressed) return;
	
	// apply initial cost
	if (!Ability) return;
	if (ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability))
	{
		if (!CrowFlurry->TryConsumeBirdMeter(CrowFlurry->InitialCostPercent)) return;
	}
	RunState();
}

void FPerformCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	
	if (!Ability) return;
	
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (!CrowFlurry) return;

	// leave running state on ability released
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		CrowFlurry->EndAbilityAnimation();
		ExitState();
	} else
	{
		Ability->BP_AbilityStarted();
		CrowFlurry->PlayAbilityAnimation();

		Ability->Blockers.IsMovementBlocked = true;
		Ability->Blockers.IsOtherAbilitiesBlocked = true;
	}
}

void FPerformCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

FEndCrowFlurryState::FEndCrowFlurryState(AAbilityBase* ability)  : FAbilityState(ability) {}

void FEndCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FEndCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
}

void FEndCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}


void ACrowFlurry::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrowFlurry, CurrFlurryRotation);
}


