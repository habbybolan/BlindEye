// Copyright (C) Nicholas Johnson 2022


#include "Boids/Flock.h"

#include "Boids/Boid.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlock::AFlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
	Super::BeginPlay();

	// used for testing boid flocking movement
	if (bSpawnOnBegin)
	{
		SpawnFlockWave();
	}
}

// Called every frame
void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformFlock();
}

void AFlock::MULT_InitializeFlock_Implementation() 
{ 
	SpawnFlockWave();
}

void AFlock::SpawnFlockWave()
{
	int rand = UKismetMathLibrary::RandomIntegerInRange(1, FlockWaveSizeMax);
	for (int i = 0; i < rand; i++)
	{
		SpawnBoidRand();
	}
	currFlocksSpawned++;
	if (currFlocksSpawned < FlockWaveCount)
		GetWorld()->GetTimerManager().SetTimer(FlockSpawnTimerHandle, this, &AFlock::SpawnFlockWave, .1f, false);
}

void AFlock::AddBoid(ABoid* newBoid)
{
	BoidsInFlock.Add(newBoid);
}

void AFlock::SpawnBoidRand()
{
	FVector location = GetActorLocation() +
		FVector(UKismetMathLibrary::RandomFloat() * XSpawnRange,
				UKismetMathLibrary::RandomFloat() * YSpawnRange,
				UKismetMathLibrary::RandomFloat() * ZSpawnRange);
	FRotator direction;

	// if a target is set, set spawn direction towards target
	if (Target.IsValid()) 
	{
		// FVector directionVec = Target->GetActorLocation() - GetActorLocation();
		// directionVec.Normalize();
		// directionVec *= 100;
		//
		// // Apply arc based on the distance target is from flock spawn location
		// float arcVerticalOffset = 0;
		// float distance = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
		// // Only apply arc for enemies that are far enough away
		// if (distance > BoidMinDistanceToNotApplyArc)
		// {
		// 	// Get percentage to apply percentage of max vertical offset based on target distance
		// 	float verticalPercentage = (distance - BoidMinDistanceToNotApplyArc) / (BoidMaxDistanceToApplyFullArc - BoidMinDistanceToNotApplyArc);
		// 	arcVerticalOffset = verticalPercentage * BoidMaxInitialVertical;
		// }
		// 	
		// directionVec += FVector::UpVector * arcVerticalOffset;
		// if (const AController* controller = GetInstigatorController())
		// {
		// 	directionVec += (controller->GetControlRotation()).Vector().RotateAngleAxis(90, FVector::ForwardVector) *
		// 				UKismetMathLibrary::RandomFloatInRange(BoidInitialHorizontalChangeMin, BoidInitialHorizontalChangeMax) *
		// 				(UKismetMathLibrary::RandomIntegerInRange(0, 1) == 0 ? -1 : 1);
		// } else
		// {
		// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "No Controller?");
		// }
		//
		//
		// direction = directionVec.Rotation();
	} else
	{
		//direction = GetActorRotation();
	}

	if (Target.IsValid())
	{
		direction = (Target->GetActorLocation() - GetActorLocation()).Rotation();
	} else
	{
		direction = GetActorRotation();
	}
	
	
	ABoid* newBoid = Cast<ABoid>(GetWorld()->SpawnActor<ABoid>(BoidType, location, direction));
	AddBoid(newBoid);
}

void AFlock::RemoveBoids()
{
	for (ABoid* boid : BoidsInFlock)
	{
		boid->Destroy();
	}
}

FVector AFlock::Separation(ABoid* boid)
{
	FVector newVelocity = FVector::ZeroVector;
	for (ABoid* neighborBoid : BoidsInFlock)
	{
		float d = FVector::Distance(boid->GetActorLocation(), neighborBoid->GetActorLocation());
		if (d > 0 && d < DesiredSeparation)
		{
			FVector diff = boid->GetActorLocation() - neighborBoid->GetActorLocation();
			newVelocity += diff;
		}
	}
	return newVelocity;
}

FVector AFlock::Cohesion(ABoid* boid)
{
	FVector avgPosition = FVector::ZeroVector;
	for (ABoid* neighborBoid : BoidsInFlock)
	{
		avgPosition += neighborBoid->GetActorLocation();
	}
	avgPosition /= BoidsInFlock.Num();
	avgPosition -= boid->GetActorLocation();
	return avgPosition;
}

FVector AFlock::Alignment(ABoid* boid)
{
	FVector alignVelocity = FVector::ZeroVector;
	for (ABoid* neighborBoid : BoidsInFlock)
	{
		alignVelocity += neighborBoid->GetVelocity();
	}
	alignVelocity /= BoidsInFlock.Num();
	return alignVelocity; 
}

FVector AFlock::TargetSeeking(ABoid* boid)
{
	if (Target.IsValid())
	{
		FVector TargetVec = Target->GetActorLocation() - boid->GetActorLocation();
		TargetVec.Normalize();
		return TargetVec;
	}
	return FVector::ZeroVector;
}

FVector AFlock::ObstacleAvoidance(ABoid* boid)
{
	FVector ObstacleAvoidVec = FVector::ZeroVector;

	UWorld* World = GetWorld();
	if (World == nullptr) return ObstacleAvoidVec;

	TArray<AActor*> OutActors;
	if (UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), SphereRadiusCheckObstacleAvoidance,
		ObjectTypesToAvoid, nullptr, TArray<AActor*>(), OutActors))
	{
		for (AActor* obstacle : OutActors)
		{
			float dist = FVector::Distance(obstacle->GetActorLocation(), boid->GetActorLocation());
		
			if (dist > 0 && dist < ObstacleRadius)
			{
				FVector	avoidVec = boid->GetActorLocation() - obstacle->GetActorLocation();
				ObstacleAvoidVec += avoidVec;
			}
		}
	}
	return ObstacleAvoidVec;
}

void AFlock::PerformFlock()
{
	for (ABoid* boid : BoidsInFlock)
	{
		FVector velocityToApply = FVector::ZeroVector;
		
		velocityToApply += Separation(boid) * SeparationStrength;
		velocityToApply += Alignment(boid) * AlignmentStrength;
		velocityToApply += Cohesion(boid) * CohesionStrength;
		velocityToApply += ObstacleAvoidance(boid) * ObstacleStrength;
		velocityToApply += TargetSeeking(boid) * TargetStrength;

		// flock reached target position, send upwards
        if (!Target.IsValid())
        {
        	velocityToApply += FVector::UpVector * 1000.f;
        }
		boid->AddForce(velocityToApply);
	}
}

FVector AFlock::CalcAveragePosition()
{
	FVector avgVector = FVector::ZeroVector;
	for (ABoid* boid : BoidsInFlock)
	{
		avgVector += boid->GetActorLocation();
	}
	avgVector /= BoidsInFlock.Num();
	return avgVector;
}

void AFlock::SetCanAttack()
{
	bCanAttack = true;
}

bool AFlock::CheckInRangeOfTarget()
{
	if (!Target.IsValid()) return false;
	
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	FVector distVector = Target->GetActorLocation() - CalcAveragePosition();
	if (distVector.Size() <= DamageRadius)
	{
		return true;
	}
	return false;
}

void AFlock::Destroyed()
{
	Super::Destroyed();
	RemoveBoids();
}

void AFlock::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( AFlock, Target );
}


