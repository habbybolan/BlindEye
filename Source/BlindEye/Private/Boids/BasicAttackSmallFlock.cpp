// Copyright (C) Nicholas Johnson 2022


#include "Boids/BasicAttackSmallFlock.h"

#include "Boids/Boid.h"
#include "Components/HealthComponent.h"
#include "Enemies/BlindEyeEnemybase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HealthInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void ABasicAttackSmallFlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FlockCheck();
}

void ABasicAttackSmallFlock::BeginPlay()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	Super::BeginPlay();

	BaseSeekingStrength = TargetStrength;

	if (GetLocalRole() == ROLE_Authority)
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);
		FVector InstigatorFwd =  ViewportRotation.Vector() * TargetDistanceFromInstigator;

		FVector TargetLocation;
		FHitResult OutHit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(world, ViewportLocation, ViewportLocation + InstigatorFwd, SpawnLineCastObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
		{
			TargetLocation = OutHit.Location;
		} else
		{
			TargetLocation = ViewportLocation + InstigatorFwd;
		}

		TArray<FVector> TargetLocations;
		TargetLocations.Add(TargetLocation);
	
		if (float DistToPlayer = FVector::Distance(TargetLocation, GetInstigator()->GetActorLocation()) < DistToPlayerToStartShrinking)
		{
			FVector NextTargetLocation = TargetLocation + GetInstigator()->GetActorForwardVector() * (DistToPlayerToStartShrinking - DistToPlayer);
			TargetLocations.Add(NextTargetLocation);
		}
		
		MULT_SetTargets(TargetLocations);
	}

	world->GetTimerManager().SetTimer(FlockCheckTimerHandle, this, &ABasicAttackSmallFlock::FlockCheck, FlockCheckDelay, true);
}

void ABasicAttackSmallFlock::MULT_SetTargets_Implementation(const TArray<FVector>& TargetLocation)
{
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for (FVector TargetLoc : TargetLocation)
	{
		TargetList.Add(world->SpawnActor<AActor>(TargetType, TargetLoc, FRotator::ZeroRotator, params));
	}
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	FVector HandLocation = OwnerCharacter->GetMesh()->GetBoneLocation(BoneSpawnLocation);
	TryStartFlock(HandLocation);
}

void ABasicAttackSmallFlock::FlockCheck()
{
	// Increase target seeking when getting closer to target
	if (IsCurrTargetValid())
	{ 
		float Percent = FVector::Distance(CalcAveragePosition(), TargetList[CurrTargetIndex].Get()->GetActorLocation()) / DistToApplyTargetSeekingIncrease;
		float StrengthIncrease = UKismetMathLibrary::FClamp(Percent, 0, 1) * MaxTargetSeekingStrengthIncrease;
		TargetStrength = BaseSeekingStrength * StrengthIncrease;
	}
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!bHasReachedTarget)
		{
			CheckForDamage();
		} else
		{
			CheckReturnedToPlayer();
		}
	}

	if (!bHasReachedTarget)
	{
		CheckTargetReached();
	} else
	{
		UpdateMaxSpeed(MovementPercentAfterReachingTarget);
		CheckShrinking();
	}
}

void ABasicAttackSmallFlock::CheckForDamage()
{
	if (!bCanAttack) return;

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(CanAttackTimerHandle, this, &AFlock::SetCanAttack, DamageCooldown, false);
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CalcAveragePosition(), DamageRadius, DamageObjectTypes, ABlindEyeEnemyBase::StaticClass(), ActorsToIgnore, OutActors);

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

void ABasicAttackSmallFlock::CheckTargetReached()
{
	if (CheckInRangeOfTarget())
	{
		CurrTargetIndex++;
		// if reached last Target, go back to player
		if (!IsCurrTargetValid())
		{
			GoBackToPlayer();
			SendEachBoidUp();
		}
	}
}

void ABasicAttackSmallFlock::GoBackToPlayer()
{
	TargetList.Add(GetInstigator());
	bHasReachedTarget = true;
	SwirlStrength = 0;
}

void ABasicAttackSmallFlock::CheckReturnedToPlayer()
{
	if (CheckInRangeOfTarget())
	{
		Destroy();
	}
}

void ABasicAttackSmallFlock::CheckShrinking()
{
	float DistToPlayer = FVector::Distance(TargetList[CurrTargetIndex]->GetActorLocation(), CalcAveragePosition());
	if (DistToPlayer < DistToPlayerToStartShrinking)
	{
		// get the percentage of distance between full shrink distance and start shrink distance
		float scale = UKismetMathLibrary::FClamp((DistToPlayer - DistFromPlayerToFullyShrink) / (DistToPlayerToStartShrinking - DistFromPlayerToFullyShrink), 0, 1);
		for (ABoid* boid : BoidsInFlock)
		{
			// prevent growing in size
			float CurrScale = boid->GetActorScale3D().Size();
			if (scale > CurrScale) return;
			
			boid->SetActorScale3D(FVector::OneVector * scale);
		}
	}
}

void ABasicAttackSmallFlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABasicAttackSmallFlock::SendEachBoidUp()
{
	for (ABoid* boid : BoidsInFlock) 
	{
		FVector UpForce = FVector::UpVector * UpForceOnTargetReached;
		boid->AddForce(UpForce);
	}
}
			