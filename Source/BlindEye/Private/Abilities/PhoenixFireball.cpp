// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireball.h"



APhoenixFireball::APhoenixFireball() : AAbilityBase()
{
	AbilityStates.Add(new FPhoenixFireballCastState(this));
}

void APhoenixFireball::DealWithDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// TODO: Deal damage to enemy if it hasn't already been hit with this attack (cone + fireball)
}

// **** States *******

// Cast Fireball State *********************

FPhoenixFireballCastState::FPhoenixFireballCastState(AAbilityBase* ability) : FAbilityState(ability) {}

void FPhoenixFireballCastState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FPhoenixFireballCastState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
}

void FPhoenixFireballCastState::ExitState()
{
	FAbilityState::ExitState();
}


