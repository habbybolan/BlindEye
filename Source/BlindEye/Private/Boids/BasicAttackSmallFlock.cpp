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
	
		TargetList.Add(world->SpawnActor<AActor>(TargetType, TargetLocation, FRotator::ZeroRotator));

		if (GetLocalRole() == ROLE_Authority)
		{
			OnRep_Target();
		}
	}

	world->GetTimerManager().SetTimer(FlockCheckTimerHandle, this, &ABasicAttackSmallFlock::FlockCheck, FlockCheckDelay, true);
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
			CheckGoBackToPlayer();
		} else
		{
			UpdateMaxSpeed(MovementPercentAfterReachingTarget);
			CheckReturnedToPlayer();
		}
	}

	if (bHasReachedTarget)
	{
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

void ABasicAttackSmallFlock::CheckGoBackToPlayer()
{
	if (CheckInRangeOfTarget())
	{
		TargetList[CurrTargetIndex]->Destroy();
		CurrTargetIndex++;

		// if reached last Target, go back to player
		if (!IsCurrTargetValid())
		{
			MULT_GoBackToPlayer();
			MULT_SendEachBoidUp();
		}
	} 
}

void ABasicAttackSmallFlock::MULT_GoBackToPlayer_Implementation()
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
	DOREPLIFETIME(ABasicAttackSmallFlock, bHasReachedTarget);
}

void ABasicAttackSmallFlock::MULT_SendEachBoidUp_Implementation()
{
	for (ABoid* boid : BoidsInFlock)
	{
		FVector UpForce = FVector::UpVector * UpForceOnTargetReached;
		boid->AddForce(UpForce);
	}
}
			