// Copyright (C) Nicholas Johnson 2022


#include "Boids/BasicAttackSmallFlock.h"

#include "Components/HealthComponent.h"
#include "Enemies/BlindEyeEnemybase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HealthInterface.h"
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

	UWorld* world = GetWorld();
	if (!world) return;
	
	// TODO: Spawn Target point distance from Instigator, using their forward position

	FVector InstigatorFwd; // =  GetInstigator()->GetControlRotation().Vector() * TargetDistanceFromInstigator;
	
	APlayerController* PlayerController = Cast<APlayerController>(GetInstigator()->GetController());
	FVector Position;
	FVector Rotation;
	if (PlayerController->DeprojectMousePositionToWorld(Position, Rotation))
	{
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingle(world, Position, Position + Rotation * 10000, MouseRayTrace,
			false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, Hit, true))
		{
			FVector HitLocation = Hit.Location + FVector::UpVector * 100;
			InstigatorFwd = HitLocation - GetOwner()->GetActorLocation();
		}
	}
	
	
	FVector SpawnLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;
	
	
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

	const IHealthInterface* InstigatorHealthInterface = Cast<IHealthInterface>(GetInstigator());
	TEAMS InstigatorTeam = InstigatorHealthInterface->Execute_GetTeam(GetInstigator());
	
	for (AActor* HitActor : OutActors)
	{
		if (const IHealthInterface* HitHealthInterface = Cast<IHealthInterface>(HitActor))
		{
			TEAMS HitTeam = HitHealthInterface->Execute_GetTeam(HitActor);
			if (InstigatorTeam != HitTeam)
			{
				UGameplayStatics::ApplyPointDamage(HitActor, DamageAmount, FVector::ZeroVector, FHitResult(), GetInstigator()->Controller, this, DamageType);
			}
		}
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
			