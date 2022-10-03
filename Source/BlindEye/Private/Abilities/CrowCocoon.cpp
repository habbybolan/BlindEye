// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowCocoon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ACrowCocoon::ACrowCocoon() : AAbilityBase()
{
	AbilityStates.Add(new UCrowCocoonStart(this));
	DamageTicks.SetNum(MaxNumberPulses);
}

void ACrowCocoon::StartHoldLogic()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(PulseTimerHandle, this, &ACrowCocoon::PerformPulse,
		MaxHoldDuration / MaxNumberPulses, true, DelayFirstPulse); 
	TimeHoldStart = GetGameTimeSinceCreation();
}

void ACrowCocoon::EndAbilityLogic()
{
	Super::EndAbilityLogic();

	UWorld* world = GetWorld();
	if (!world) return;
	 
	world->GetTimerManager().ClearTimer(PulseTimerHandle);

	UGameplayStatics::ApplyPointDamage(GetInstigator(), 1, FVector::ZeroVector,
		FHitResult(), GetInstigator()->GetController(), GetInstigator(), MainDamageType);
	UGameplayStatics::ApplyRadialDamage(world, DamageTicks[CalcPulseIndex()], GetInstigator()->GetActorLocation(), Radius, MainDamageType,
		TArray<AActor*>(), GetInstigator());
	bReachedFinalPulse = false;
}

void ACrowCocoon::PerformPulse()
{
	UWorld* world = GetWorld();
	if (!world) return;

	if (bReachedFinalPulse) return;
	
	uint8 pulseIndex = CalcPulseIndex();
	if (pulseIndex >= MaxNumberPulses - 1)
	{
		bReachedFinalPulse = true;
	}
	
	BP_AbilityInnerState(pulseIndex + 1);
	// perform taunt on first pulse index
	if (pulseIndex == 0)
	{
		UGameplayStatics::ApplyRadialDamage(world, DamageTicks[CalcPulseIndex()], GetInstigator()->GetActorLocation(), Radius, FirstPulseDamageType,
		TArray<AActor*>(), GetInstigator());
	}

	// consume bird meter
	if (!TryConsumeBirdMeter(CostPercentPerSec * MaxHoldDuration / MaxNumberPulses))
	{
		// if out of bird meter, simulate releasing button
		AbilityStates[CurrState]->HandleInput(EAbilityInputTypes::Released);
	}
}

uint8 ACrowCocoon::CalcPulseIndex()
{
	float timeBetweenPulses = MaxHoldDuration / MaxNumberPulses;
	float currHoldTime = GetGameTimeSinceCreation() - TimeHoldStart;
	uint8 pulseIndex = currHoldTime / timeBetweenPulses;
	uint8 LastIndex = MaxNumberPulses - 1;
	return FMath::Min(pulseIndex, LastIndex);
}


// **** States *******

// Cocoon Start State *********************

UCrowCocoonStart::UCrowCocoonStart(AAbilityBase* ability): FAbilityState(ability) {}

void UCrowCocoonStart::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// apply initial cost
	if (!Ability) return;
	if (ACrowCocoon* CrowCocoon = Cast<ACrowCocoon>(Ability))
	{
		if (!CrowCocoon->TryConsumeBirdMeter(CrowCocoon->InitialCostPercent)) return;
	}
	// only enter ability on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void UCrowCocoonStart::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (!Ability) return;
	Ability->BP_AbilityStarted();
	Ability->Blockers.IsMovementSlowBlocked = true;
	Ability->Blockers.MovementSlowAmount = 0.2;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		ExitState();
	} else
	{
		// perform holding logic
		if (ACrowCocoon* Cocoon = Cast<ACrowCocoon>(Ability))
		{
			Cocoon->StartHoldLogic();
		}
	}
}

void UCrowCocoonStart::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
}


