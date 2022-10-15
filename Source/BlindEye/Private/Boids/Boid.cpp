// Copyright (C) Nicholas Johnson 2022


#include "Boids/Boid.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABoid::ABoid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>("Root Scene");
	RootComponent = RootScene;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Boid Mesh");
	Mesh->SetCollisionProfileName("OverlapAll");
	Mesh->SetEnableGravity(false);
	Mesh->SetupAttachment(RootComponent);
	
	BoidMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Boid Movement");
	BoidMovement->SetUpdatedComponent(RootComponent);
	BoidMovement->SetIsReplicated(true);
	BoidMovement->bRotationFollowsVelocity = true;
	BoidMovement->ProjectileGravityScale = 0.0f;
	BoidMovement->Velocity = FVector(1000, 0, 0);

	PrevVelocity = BoidMovement->Velocity;

}

float ABoid::GetX()
{
	return GetActorLocation().X;
}

float ABoid::GetY()
{
	return GetActorLocation().Y;
}

float ABoid::GetZ()
{
	return GetActorLocation().Z;
}

FVector ABoid::GetVelocity() const
{
	return BoidMovement->Velocity;
}

void ABoid::SetVelocity(FVector& velocity)
{
	BoidMovement->Velocity = velocity;
}

void ABoid::AddForce(FVector& velocity)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	FVector CurrVelocity = BoidMovement->Velocity;
	CurrVelocity.Normalize();
	
	FVector TargetVelocity = velocity;
	TargetVelocity.Normalize();
	
	float AimAtAngle = (acosf(FVector::DotProduct(CurrVelocity, TargetVelocity))) * World->GetDeltaSeconds();
	
	float LerpPercent;
	if (AimAtAngle > MaxRotationAmount)
	{
		LerpPercent = MaxRotationAmount / AimAtAngle;
	} else
	{
		LerpPercent = 1;
	}
	
	FRotator LerpedRot = UKismetMathLibrary::RLerp(BoidMovement->Velocity.Rotation(), velocity.Rotation(), LerpPercent, true);
	BoidMovement->AddForce(LerpedRot.Vector() * velocity.Size());
}

void ABoid::UpdateMaxSpeed(float PercentOfMaxSpeed)
{
	CurrMaxSpeed = MaxSpeed * PercentOfMaxSpeed;
}

void ABoid::InitializeBoid(FVector Location, FRotator Rotation)
{
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	BoidMovement->Velocity = GetActorForwardVector() * 1000;
	GetWorldTimerManager().SetTimer(SpawnSizeGrowTimerHandle, this, &ABoid::InitialSpawnSizeGrow, SizeGrowTimerDelay, true);
}

void ABoid::DisableActor(bool bDisableActor)
{
	bIsDisabled = bDisableActor;
	Mesh->SetVisibility(true);
	SetActorEnableCollision(!bDisableActor);
	SetActorTickEnabled(!bDisableActor);

	BoidMovement->SetComponentTickEnabled(!bDisableActor);\
	if (bDisableActor)
	{
		BoidMovement->Velocity = FVector::ZeroVector;
	}
}

bool ABoid::GetIsActorDisabled()
{
	return bIsDisabled;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	CurrMaxSpeed = MaxSpeed;
	SetLifeSpan(100);
	SetActorScale3D(FVector::OneVector * SpawnScaleSize);
	
	SetActorRotation(GetActorRotation().Add(0, 0, 90));

	// force tick to occur after Flock tick
	AddTickPrerequisiteActor(GetInstigator());
}

void ABoid::InitialSpawnSizeGrow()
{
	CurrTimerSizeGrow += SizeGrowTimerDelay;
	if (CurrTimerSizeGrow >= TimeUntilFullSizeOnSpawn)
		CurrTimerSizeGrow = TimeUntilFullSizeOnSpawn;

	// Scale size of boid linearly
	SetActorScale3D(UKismetMathLibrary::VLerp(FVector::OneVector * SpawnScaleSize, FVector::OneVector, CurrTimerSizeGrow / TimeUntilFullSizeOnSpawn));

	// stop timer if reached max size
	if (CurrTimerSizeGrow >= TimeUntilFullSizeOnSpawn)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
		World->GetTimerManager().ClearTimer(SpawnSizeGrowTimerHandle);
	}
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// TODO: Put on timer
	HorizontalRotation();

	if (BoidMovement->Velocity.Size() > CurrMaxSpeed)
	{
		BoidMovement->Velocity *= 1 - ((BoidMovement->Velocity.Size() - CurrMaxSpeed) / BoidMovement->Velocity.Size());
	}

	// store curr velocity to calculate movement difference next frame
	PrevVelocity = BoidMovement->Velocity;

}

void ABoid::HorizontalRotation()
{
	FVector currVelocity = BoidMovement->Velocity;
	
	// get angle difference on XY plane for curr and prev velocity of boid
	const FVector2D currVelocity2D = FVector2D(currVelocity.X, currVelocity.Z);
	const FVector2D prevVelocity2D = FVector2D(PrevVelocity.X, PrevVelocity.Z);
	float crossProd2D = UKismetMathLibrary::CrossProduct2D(currVelocity2D, prevVelocity2D);
	float angleOnXZPlane = UKismetMathLibrary::RadiansToDegrees(asin(crossProd2D / (currVelocity2D.Size() * prevVelocity2D.Size()) ));

	// Rotate either based on XY plane if turning, otherwise rotate back to resting position
	float NewRoll = UKismetMathLibrary::Lerp(
			Mesh->GetRelativeRotation().Roll,
			abs(angleOnXZPlane) < .1 ? 0 :
				angleOnXZPlane < 0 ? -MaxRollRotation : MaxRollRotation,
			RollRotationSpeed);
	
	FRotator NewRollRot = FRotator(
			Mesh->GetRelativeRotation().Pitch,
			Mesh->GetRelativeRotation().Yaw,
			NewRoll);
	
	Mesh->SetRelativeRotation(NewRollRot);
}

