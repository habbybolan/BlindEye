// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABurrowerProjectile::ABurrowerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SphereComponent =  CreateDefaultSubobject<USphereComponent>("SphereComponent");
	RootComponent = SphereComponent;

	Projectile = CreateDefaultSubobject<UStaticMeshComponent>("Projectile");
	Projectile->SetupAttachment(RootComponent);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>("Movement");
	Movement->ProjectileGravityScale = 1;
	Movement->InitialSpeed = 0;
	Movement->Velocity = FVector::ZeroVector;
	Movement->ProjectileGravityScale = 1;
}

// Called when the game starts or when spawned
void ABurrowerProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentHit.AddDynamic(this, &ABurrowerProjectile::OnContact);
}

void ABurrowerProjectile::LaunchProjectile_Implementation(FVector TargetPosition)
{
	float angle = UKismetMathLibrary::DegreesToRadians(InitialAngle);

	// position of this object and the target on the same plane
	FVector planarTarget = FVector(TargetPosition.X, TargetPosition.Y, 0);
	FVector planarPosition = FVector(GetActorLocation().X, GetActorLocation().Y, 0);

	// Planar distance between objects
	float distance = FVector::Distance(planarTarget, planarPosition);
	// distance along the y axis between objects
	float ZOffset = GetActorLocation().Z - TargetPosition.Z;

	float initialVelocity = (1 / UKismetMathLibrary::Cos(angle)) * UKismetMathLibrary::Sqrt((0.5f * -1 * Movement->GetGravityZ() * distance * distance)  / (distance * UKismetMathLibrary::Tan(angle) + ZOffset));
	
	FVector velocity = FVector(initialVelocity * UKismetMathLibrary::Cos(angle), 0, initialVelocity * UKismetMathLibrary::Sin(angle));

	// Rotate our velocity to match the direction between two objects
	float angleBetweenObjects = UKismetMathLibrary::RadiansToDegrees ((planarTarget - planarPosition).HeadingAngle());
	
	FVector finalVelocity = velocity.RotateAngleAxis(angleBetweenObjects, FVector::UpVector);

	// Fire
	Movement->Velocity = finalVelocity;
}

void ABurrowerProjectile::OnContact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Projectile->SetVisibility(false);
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMultiForObjects(World, GetActorLocation(), GetActorLocation(), DamageRadius, ObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);

		for (FHitResult TraceHit : OutHits)
		{
			if (GetInstigator() && GetInstigator()->GetController())
			{
				UGameplayStatics::ApplyPointDamage(TraceHit.GetActor(), DamageAmount, TraceHit.Location, TraceHit, GetInstigator()->GetController(), this, DamageType);
			}
		}
		BP_OnContact();
	}
}

void ABurrowerProjectile::DelayedDestruction()
{
	Destroy();
}

