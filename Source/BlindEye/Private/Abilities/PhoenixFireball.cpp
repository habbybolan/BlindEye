// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireball.h"
#include "Kismet/GameplayStatics.h"


APhoenixFireball::APhoenixFireball() : AAbilityBase()
{
	AbilityStates.Add(new FPhoenixFireballCastState(this));
}

void APhoenixFireball::DealWithDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit, float DamageToApply)
{
	// prevent damage on same actor twice
	if (IDsOfHitActors.Contains(OtherActor->GetUniqueID())) return;
	
	IDsOfHitActors.Add(OtherActor->GetUniqueID());
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageToApply, NormalImpulse, Hit, GetInstigator()->GetController(), GetInstigator(), DamageType);
}

void APhoenixFireball::CastFireCone()
{
	// TODO:
}

void APhoenixFireball::CastFireball()
{
	// TODO:
}

void APhoenixFireball::EndAbilityLogic()
{
	IDsOfHitActors.Empty();
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
	if (!Ability) return;
	APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability);
	if (!PhoenixFireball) return;

	PhoenixFireball->CastFireball();
	PhoenixFireball->CastFireCone();
	ExitState();
}

void FPhoenixFireballCastState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability) Ability->EndCurrState();
}


