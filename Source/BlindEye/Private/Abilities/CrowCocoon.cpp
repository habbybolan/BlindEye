// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowCocoon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ACrowCocoon::ACrowCocoon() : AAbilityBase()
{
	AbilityStates.Add(new UCrowCocoonStart(this));
	AbilityStates.Add(new UCrowPulseState(this));
	PulseParticles.SetNum(MaxNumberPulses, false);
	SpawnedPulseParticles.SetNum(MaxNumberPulses, false);
}

void ACrowCocoon::StartHoldLogic()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(PulseTimerHandle, this, &ACrowCocoon::MULT_PerformPulse,
		MaxHoldDuration / MaxNumberPulses, true, DelayFirstPulse); 
	TimeHoldStart = GetGameTimeSinceCreation();
}

void ACrowCocoon::EndHold()
{
	float CurrTime = GetGameTimeSinceCreation();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Emerald, "Held for: " + FString::SanitizeFloat(CurrTime - TimeHoldStart));

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().ClearTimer(PulseTimerHandle);
	MULT_PerformExplosionPulse();
}

void ACrowCocoon::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	if (SpawnedFullyChargedParticle) SpawnedFullyChargedParticle->Deactivate();
}

void ACrowCocoon::MULT_PerformExplosionPulse_Implementation()
{
	SpawnedExplosionPulse = UNiagaraFunctionLibrary::SpawnSystemAttached(ExplosionPulse, GetInstigator()->GetRootComponent(), NAME_None,
		GetInstigator()->GetActorLocation(), FRotator::ZeroRotator, FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

void ACrowCocoon::MULT_PerformPulse_Implementation() 
{
	float timeBetweenPulses = MaxHoldDuration / MaxNumberPulses;
	float currHoldTime = GetGameTimeSinceCreation() - TimeHoldStart;
	uint8 pulseIndex = (currHoldTime / timeBetweenPulses);
	if (pulseIndex >= MaxNumberPulses) return; // Something went wrong

	SpawnedPulseParticles[pulseIndex] = UNiagaraFunctionLibrary::SpawnSystemAttached(PulseParticles[pulseIndex], GetInstigator()->GetRootComponent(), NAME_None,
		GetInstigator()->GetActorLocation(), FRotator::ZeroRotator, FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);

	// Play extra particle when last pulse reached
	if (pulseIndex == MaxNumberPulses - 1)
	{
		SpawnedFullyChargedParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(FullyChargedParticle, GetInstigator()->GetRootComponent(), NAME_None,
		GetInstigator()->GetActorLocation(), FRotator::ZeroRotator, FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);

		UWorld* world = GetWorld();
		if (!world) return;
		world->GetTimerManager().ClearTimer(PulseTimerHandle);
	}
}

// **** States *******

// Cocoon Start State *********************

UCrowCocoonStart::UCrowCocoonStart(AAbilityBase* ability): FAbilityState(ability) {}

void UCrowCocoonStart::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// only enter ability on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void UCrowCocoonStart::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		ExitState();
	} else
	{
		// perform holding logic
		if (!Ability) return;
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
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Cocoon Pulse State *********************

UCrowPulseState::UCrowPulseState(AAbilityBase* ability) : FAbilityState(ability) {}

void UCrowPulseState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void UCrowPulseState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	// Perform explosion attack
	if (!Ability) return;
	if (ACrowCocoon* Cocoon = Cast<ACrowCocoon>(Ability))
	{
		Cocoon->EndHold();
	}
	ExitState();
}

void UCrowPulseState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
}


