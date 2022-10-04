// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


APhoenixFireball::APhoenixFireball() : AAbilityBase()
{
	AbilityStates.Add(new FPhoenixFireballCastState(this));
}

void APhoenixFireball::DealWithDamage(AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit, float DamageToApply)
{
	if (OtherActor == nullptr) return;
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
		GetInstigator()->GetControlRotation(), ConeTraceObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None,
		OutHits, true);

	FVector UserLocation = GetInstigator()->GetActorLocation();
	for (FHitResult ConeHit : OutHits)
	{
		// some surfaces return null for actor
		if (ConeHit.GetActor() == nullptr) return;
		
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
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.Instigator = GetInstigator();
	params.Owner = this;

	FVector spawnLocation = GetInstigator()->GetActorLocation() + GetInstigator()->GetControlRotation().Vector() * FVector(100, 100, 0);
	FireballCast = world->SpawnActor<APhoenixFireballCast>(FireballCastType, spawnLocation, GetInstigator()->GetControlRotation(), params);
	FireballCast->GetSphereComponent()->OnComponentHit.AddDynamic(this, &APhoenixFireball::OnFireballCastHit);
}

void APhoenixFireball::OnFireballCastHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	if (GetLocalRole() < ROLE_Authority) return;
	if (!FireballCast) return;

	// Area damage from fireball cast colliding / expiring and exploding
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(World, FireballCast->GetActorLocation(), FireballCast->GetActorLocation() + FireballCast->GetActorForwardVector() * 5,
		Damage, ConeTraceObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	for (FHitResult ExplosionHit : OutHits)
	{
		DealWithDamage(ExplosionHit.Actor.Get(), ExplosionHit.ImpactNormal, ExplosionHit, Damage);
	}
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
	if (!Ability) return;
	if (APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability))
	{
		if (!PhoenixFireball->TryConsumeBirdMeter(PhoenixFireball->CostPercent)) return;
	}
	if (abilityUsageType != EAbilityInputTypes::Pressed) return;
	RunState();
}

void FPhoenixFireballCastState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (!Ability) return;
	Ability->BP_AbilityStarted();
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


