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
	BoidMovement->MaxSpeed = 1500.f;
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
	BoidMovement->AddForce(velocity);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(100);
	
	SetActorRotation(GetActorRotation().Add(0, 0, 90));

	// force tick to occur after Flock tick
	AddTickPrerequisiteActor(GetInstigator());
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// TODO: Put on timer
	HorizontalRotation();

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
