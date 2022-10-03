// Copyright (C) Nicholas Johnson 2022


#include "Boids/BasicAttackSmallFlock.h"

#include "Boids/Boid.h"
#include "Components/HealthComponent.h"
#include "Enemies/BlindEyeEnemybase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HealthInterface.h"

void ABasicAttackSmallFlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		CheckForDamage();
	}

	if (!bHasReachedTarget)
	{
		CheckGoBackToPlayer();
	} else
	{
		CheckReturnedToPlayer();
	}
}

void ABasicAttackSmallFlock::BeginPlay()
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	Super::BeginPlay();
	// TODO: Spawn Target point distance from Instigator, using their forward position
	FVector InstigatorFwd =  GetInstigator()->GetControlRotation().Vector() * TargetDistanceFromInstigator;
	FVector SpawnLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	UWorld* world = GetWorld();
	if (!world) return;
	
	Target = world->SpawnActor<AActor>(TargetType, SpawnLocation, FRotator::ZeroRotator);
	// manually call initialize flock if server, client calls once target is replicated
	if (GetLocalRole() == ROLE_Authority)
	{
		InitializeFlock();
	}
}

void ABasicAttackSmallFlock::CheckForDamage()
{
	if (!bCanAttack) return;

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(CanAttackTimerHandle, this, &AFlock::SetCanAttack, DamageCooldown, false);
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CalcAveragePosition(), DamageRadius, ObjectTypes, ABlindEyeEnemyBase::StaticClass(), ActorsToIgnore, OutActors);

	IHealthInterface* InstigatorHealthInterface = Cast<IHealthInterface>(GetInstigator());
	TEAMS InstigatorTeam = InstigatorHealthInterface->GetTeam();
	
	for (AActor* HitActor : OutActors)
	{
		if (IHealthInterface* HitHealthInterface = Cast<IHealthInterface>(HitActor))
		{
			TEAMS HitTeam = HitHealthInterface->GetTeam();
			if (InstigatorTeam != HitTeam)
			{
				UGameplayStatics::ApplyPointDamage(HitActor, DamageAmount, FVector::ZeroVector, FHitResult(), GetInstigator()->Controller, this, DamageType);
			}
		}
	}
}

void ABasicAttackSmallFlock::CheckGoBackToPlayer()
{
	if (CheckInRangeOfTarget())
	{
		bHasReachedTarget = true;
		Target->Destroy();
		Target = nullptr;

		SendEachBoidUp();
		Target = GetInstigator();
	}
}

void ABasicAttackSmallFlock::CheckReturnedToPlayer()
{
	if (CheckInRangeOfTarget())
	{
		Destroy();
	}
}

void ABasicAttackSmallFlock::SendEachBoidUp()
{
	for (ABoid* boid : BoidsInFlock)
	{
		FVector UpForce = FVector::UpVector * UpForceOnTargetReached;
		boid->AddForce(UpForce);
	}
}
			