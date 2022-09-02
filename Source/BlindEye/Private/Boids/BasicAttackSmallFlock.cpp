// Copyright (C) Nicholas Johnson 2022


#include "Boids/BasicAttackSmallFlock.h"

#include "Enemies/BlindEyeEnemybase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DamageTypes/BaseDamageType.h"

void ABasicAttackSmallFlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		CheckRemoveTarget();
		CheckForDamage();
	}
}

void ABasicAttackSmallFlock::BeginPlay()
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	Super::BeginPlay();
	// TODO: Spawn Target point distance from Instigator, using their forward position
	FVector InstigatorFwd =  GetInstigator()->GetActorForwardVector() * TargetDistanceFromInstigator;
	FVector SpawnLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	UWorld* world = GetWorld();
	if (!world) return;
	
	Target = world->SpawnActor<AActor>(TargetType, SpawnLocation, FRotator::ZeroRotator);
	InitializeFlock();
}

void ABasicAttackSmallFlock::CheckForDamage()
{
	if (!bCanAttack) return;

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(CanAttackTimerHandle, this, &AFlock::SetCanAttack, DamageCooldown, false);
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CalcAveragePosition(), DamageRadius, ObjectTypes, ABlindEyeEnemyBase::StaticClass(), ActorsToIgnore, OutActors);

	for (AActor* HitActor : OutActors)
	{
		UGameplayStatics::ApplyPointDamage(HitActor, DamageAmount, FVector::ZeroVector, FHitResult(), GetInstigator()->Controller, this, DamageType);
	}
}

void ABasicAttackSmallFlock::CheckRemoveTarget()
{
	if (CheckInRangeOfTarget())
	{
		Target->Destroy();
		Target = nullptr;
		
		// TODO: Temp destroy, remove later
		Destroy();
	}
}
			