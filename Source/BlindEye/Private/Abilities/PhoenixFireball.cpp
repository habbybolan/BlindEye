// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireball.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


APhoenixFireball::APhoenixFireball() : AAbilityBase()
{
	AbilityStates.Add(new FPhoenixFireballCastState(this));
}

void APhoenixFireball::DealWithDamage(AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit, float DamageToApply)
{
	// prevent damage on same actor twice
	if (IDsOfHitActors.Contains(OtherActor->GetUniqueID())) return;
	
	IDsOfHitActors.Add(OtherActor->GetUniqueID());
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageToApply, NormalImpulse, Hit, GetInstigator()->GetController(), GetInstigator(), DamageType);
}

void APhoenixFireball::CastFireCone()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FVector ViewportLocation;
	FRotator ViewportRotation;
	GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);

	FVector EndLocation = ViewportLocation + ViewportRotation.Vector() * 1000;
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(world, ViewportLocation, EndLocation, LineTraceObjectTypes, false,
		TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
	{
		EndLocation = OutHit.Location;
	}

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), EndLocation, FVector(0, ConeWidth / 2, ConeLength / 2),
		GetInstigator()->GetControlRotation(), ConeTraceObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration,
		OutHits, true);

	FVector UserLocation = GetInstigator()->GetActorLocation();
	for (FHitResult ConeHit : OutHits)
	{
		// check if hit result in boxTrace is within the player's cone
		float AngleToUser = acos(UKismetMathLibrary::Dot_VectorVector(UserLocation, ConeHit.Location) / (UserLocation.Size() * ConeHit.Location.Size()));
		if (abs(AngleToUser) <= ConeHalfAngleDeg)
		{
			DealWithDamage(ConeHit.GetActor(), ConeHit.ImpactNormal, ConeHit, Damage);
		}
	}
}

void APhoenixFireball::CastFireball()
{
	// TODO:
}

void APhoenixFireball::EndAbilityLogic()
{
	AAbilityBase::EndAbilityLogic();
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


