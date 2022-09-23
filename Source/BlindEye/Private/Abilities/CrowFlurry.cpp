// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowFlurry.h"

#include "Interfaces/HealthInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ACrowFlurry::ACrowFlurry()
{
	AbilityStates.Add(new UFirstCrowFlurryState(this));
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	CurrFlurryRotation = GetInstigator()->GetControlRotation();
	world->GetTimerManager().SetTimer(CrowFlurryTimerHandle, this, &ACrowFlurry::PerformCrowFlurry, 0.2f, true);
	
	MULT_SpawnCrowFlurry(GetInstigator()->GetControlRotation());
}

void ACrowFlurry::MULT_SpawnCrowFlurry_Implementation(FRotator rotation)
{
	MULT_SpawnCrowFlurryHelper();
}

void ACrowFlurry::PerformCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FRotator TargetRotation = GetInstigator()->GetControlRotation();

	CurrFlurryRotation = UKismetMathLibrary::RLerp(CurrFlurryRotation, TargetRotation, 0.5, true);
 
	FVector InstigatorFwd =  CurrFlurryRotation.Vector() * Range;
	FVector TargetLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), TargetLocation, FVector(0, Width / 2, Height / 2),
		GetInstigator()->GetControlRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true,
		FLinearColor::Red, FLinearColor::Green, 0.2f);
	
	for (FHitResult Hit : OutHits)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamagePerSec * 0.2f, FVector::ZeroVector,
					FHitResult(), GetInstigator()->Controller, this, DamageType);
	}

	// consume bird meter
	if (!TryConsumeBirdMeter(CostPercentPerSec * 0.2f))
	{
		// if out of bird meter, simulate releasing button
		AbilityStates[CurrState]->HandleInput(EAbilityInputTypes::Pressed);
	}
}

void ACrowFlurry::MULT_DestroyCrowFlurry_Implementation()
{
	MULT_DestroyCrowFlurryHelper();
}

void ACrowFlurry::StopCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	MULT_DestroyCrowFlurry();
	
	// TODO: Stop particles/sound...
}

void ACrowFlurry::DestroyParticles()
{
	if (SpawnedCrowFlurryParticle)
	{
		SpawnedCrowFlurryParticle->DestroyComponent();
	}
}

void ACrowFlurry::EndAbilityLogic()
{
	Super::EndAbilityLogic();

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
	world->GetTimerManager().ClearTimer(CrowFlurryParticleDestroyTimerHandle);
}

// **** States *******

// First Crow Flurry state *********************

UFirstCrowFlurryState::UFirstCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

void UFirstCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	if (abilityUsageType != EAbilityInputTypes::Pressed) return;
	
	// apply initial cost
	if (!Ability) return;
	if (ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability))
	{
		if (!CrowFlurry->TryConsumeBirdMeter(CrowFlurry->InitialCostPercent)) return;
		CrowFlurry->StartCrowFlurry();
	}
	RunState();
}

void UFirstCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	
	if (!Ability) return;
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (!CrowFlurry) return;

	Ability->Blockers.IsMovementSlowBlocked = true;
	Ability->Blockers.MovementSlowAmount = CrowFlurry->MovementSlowAmount;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	// leave running state on ability released
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		CrowFlurry->StopCrowFlurry();
		ExitState();
	}
}

void UFirstCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}


