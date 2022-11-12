// Copyright (C) Nicholas Johnson 2022


#include "Boids/Flock.h"

#include "Boids/Boid.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Tools/LocalPlayerSubsystem_Pooling.h"


// Sets default values
AFlock::AFlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
}
 
void AFlock::TryStartFlock(FVector spawnLocation)
{
	if (bFlockInitialized) return;
	
	bFlockInitialized = true;
	SpawnLocation = spawnLocation;
	InitializeFlock();
}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// used for testing boid flocking movement
	if (bSpawnOnBegin)
	{
		SpawnFlockWave();
	}

	World->GetTimerManager().SetTimer(PerformFlockTimerHandle, this, &AFlock::PerformFlock, PerformFlockDelay, true);
}

// Called every frame
void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformFlock();
}

void AFlock::InitializeFlock()  
{ 
	SpawnFlockWave();
}

void AFlock::SpawnFlockWave()
{
	int FlockSizeAlter = FlockWaveSize;
	if (FlockSizeVariation > 0)
	{
		// Get random variation amount
		int rand = UKismetMathLibrary::RandomIntegerInRange(0, FlockSizeVariation);
		// Check if pos/neg rand variation
		int PosNeg = UKismetMathLibrary::RandomIntegerInRange(0, 1);
		if (PosNeg == 1) rand *= -1;
		// apply variation, allow min flock size of 1
		FlockSizeAlter = UKismetMathLibrary::Max(1, FlockSizeAlter + PosNeg);
	}
	
	for (int i = 0; i < FlockSizeAlter; i++)
	{
		SpawnBoidRand();
	}
	currFlocksSpawned++;
	if (currFlocksSpawned < FlockWaveCount)
		GetWorld()->GetTimerManager().SetTimer(FlockSpawnTimerHandle, this, &AFlock::SpawnFlockWave, DelayBetweenWaveSpawning, false);
}

void AFlock::AddBoid(ABoid* newBoid)
{
	BoidsInFlock.Add(newBoid);
}

void AFlock::SpawnBoidRand()
{
	AActor* OwningActor = Cast<ACharacter>(GetOwner());
	
	FVector location = SpawnLocation +
		FVector(UKismetMathLibrary::RandomFloat() * XSpawnRange,
				UKismetMathLibrary::RandomFloat() * YSpawnRange,
				UKismetMathLibrary::RandomFloat() * ZSpawnRange);
	FRotator direction;

	if (IsCurrTargetValid())
	{
		direction = (TargetList[CurrTargetIndex]->GetActorLocation() - OwningActor->GetActorLocation()).Rotation();
	} else
	{
		direction = OwningActor->GetActorForwardVector().Rotation();
	}

	// ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(0);
	// ULocalPlayerSubsystem_Pooling* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerSubsystem_Pooling>();
	// if (LocalPlayerSubsystem) 
	// {
	// 	if (AActor* BoidActor = LocalPlayerSubsystem->GetPooledActor(TagPoolType))
	// 	{
	// 		if (ABoid* boid = Cast<ABoid>(BoidActor))
	// 		{
	// 			AddBoid(boid);
	// 			boid->InitializeBoid(location, direction);
	// 		}
	// 	}
	// }
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	ABoid* Boid =  World->SpawnActor<ABoid>(BoidType, location, direction);
	if (Boid)
	{
		AddBoid(Boid);
		Boid->InitializeBoid(location, direction);
	}
}

bool AFlock::IsCurrTargetValid()
{
	return TargetList.Num() - 1 >= CurrTargetIndex && TargetList[CurrTargetIndex].IsValid();
}

void AFlock::RemoveBoids()
{
	for (ABoid* boid : BoidsInFlock)
	{
		// ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(0);
		// ULocalPlayerSubsystem_Pooling* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerSubsystem_Pooling>();
		// if (LocalPlayerSubsystem)
		// {
		// 	LocalPlayerSubsystem->ReturnActorToPool(boid);
		// }
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
	if (IsCurrTargetValid())
	{
		FVector TargetVec = TargetList[CurrTargetIndex]->GetActorLocation() - boid->GetActorLocation();
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

FVector AFlock::Noise(ABoid* boid)
{
	FVector CurrForward = boid->BoidMovement->Velocity;
	float RandRotation = UKismetMathLibrary::RandomFloatInRange(0, NoiseAngleVariation);
	uint8 RandInt = UKismetMathLibrary::RandomIntegerInRange(0, 1);
	FVector Axis = RandInt == 0 ? FVector(1, 0, 0) : FVector(0, 1, 0);
	return CurrForward.RotateAngleAxis(RandRotation, Axis);
}

FVector AFlock::Swirling(ABoid* boid, FVector AvgPosition, FVector Alignment)
{
	FVector RotationVec = UKismetMathLibrary::Cross_VectorVector(boid->GetActorLocation() - AvgPosition, Alignment);
	// flip the direction the swirl is rotating
	if (bFlippedSwirlRotation) RotationVec *= -1;
	return RotationVec;
}

void AFlock::UpdateMaxSpeed(float PercentToChangeSpeed)
{
	for (ABoid* boid : BoidsInFlock)
	{
		boid->UpdateMaxSpeed(PercentToChangeSpeed);
	}
}

void AFlock::PerformFlock()
{
	FVector AvgLocation = CalcAveragePosition();
	for (ABoid* boid : BoidsInFlock)
	{
		FVector velocityToApply = FVector::ZeroVector;
		
		velocityToApply += Separation(boid) * SeparationStrength;
		FVector AlignmentVec = Alignment(boid);
		velocityToApply += AlignmentVec * AlignmentStrength;
		velocityToApply += Cohesion(boid) * CohesionStrength;
		velocityToApply += ObstacleAvoidance(boid) * ObstacleStrength;
		velocityToApply += TargetSeeking(boid) * TargetStrength;
		velocityToApply += Noise(boid) * NoiseStrength;
		velocityToApply += Swirling(boid, AvgLocation, AlignmentVec) * SwirlStrength;

		// flock reached target position, send upwards
        if (!IsCurrTargetValid())
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
	if (!IsCurrTargetValid()) return false;
	
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	FVector distVector = TargetList[CurrTargetIndex]->GetActorLocation() - CalcAveragePosition();
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


