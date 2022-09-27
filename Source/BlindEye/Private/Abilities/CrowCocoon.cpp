// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowCocoon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ACrowCocoon::ACrowCocoon() : AAbilityBase()
{
	AbilityStates.Add(new UCrowCocoonStart(this));
	AbilityStates.Add(new UCrowPulseState(this));
	
	PulseParticles.SetNum(MaxNumberPulses, false);
	SpawnedPulseParticles.SetNum(MaxNumberPulses, false);
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

	UWorld* world = GetWorld();
	if (!world) return;

	UGameplayStatics::ApplyPointDamage(GetInstigator(), 1, FVector::ZeroVector,
		FHitResult(), GetInstigator()->GetController(), GetInstigator(), MainDamageType);
	UGameplayStatics::ApplyRadialDamage(world, DamageTicks[CalcPulseIndex()], GetInstigator()->GetActorLocation(), Radius, MainDamageType,
		TArray<AActor*>(), GetInstigator());
}

void ACrowCocoon::PerformPulse()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	uint8 pulseIndex = CalcPulseIndex();
	BP_AbilityInnerState(pulseIndex + 1);
	// perform taunt on first pulse index
	if (pulseIndex == 0)
	{
		UGameplayStatics::ApplyRadialDamage(world, DamageTicks[CalcPulseIndex()], GetInstigator()->GetActorLocation(), Radius, FirstPulseDamageType,
		TArray<AActor*>(), GetInstigator());
	}
	MULT_PerformPulseParticles(CalcPulseIndex());

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

void ACrowCocoon::MULT_PerformExplosionPulse_Implementation()
{
	SpawnedExplosionPulse = UNiagaraFunctionLibrary::SpawnSystemAttached(ExplosionPulse, GetInstigator()->GetRootComponent(), NAME_None,
		GetInstigator()->GetActorLocation(), FRotator::ZeroRotator, FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

void ACrowCocoon::MULT_PerformPulseParticles_Implementation(uint8 pulseIndex) 
{
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
	Ability->Blockers.IsMovementBlocked = true;
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


