// Copyright (C) Nicholas Johnson 2022


#include "Navmesh/NavLinkProxy_Jump.h"

#include "BlindEyeBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ANavLinkProxy_Jump::ANavLinkProxy_Jump(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PointLinks.Empty();
	bSmartLinkIsRelevant = true;
}

void ANavLinkProxy_Jump::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &ANavLinkProxy_Jump::OnSmartLinkReachedHelper);
}

FVector ANavLinkProxy_Jump::CalculateDownwardVectorImpulse(ABlindEyeBaseCharacter* Enemy, FVector TargetPosition, float Angle)
{
	float angle = UKismetMathLibrary::DegreesToRadians(Angle);

	// position of this object and the target on the same plane
	FVector planarTarget = FVector(TargetPosition.X, TargetPosition.Y, 0);
	FVector planarPosition = FVector(Enemy->GetActorLocation().X, Enemy->GetActorLocation().Y, 0);

	// Planar distance between objects
	float distance = FVector::Distance(planarTarget, planarPosition);
	// distance along the y axis between objects
	float ZOffset = Enemy->GetActorLocation().Z - TargetPosition.Z;

	float initialVelocity = (1 / UKismetMathLibrary::Cos(angle)) *
		UKismetMathLibrary::Sqrt(
			(0.5f * -1 * Enemy->GetCharacterMovement()->GetGravityZ() * distance * distance)
			/ (distance * UKismetMathLibrary::Tan(angle) + ZOffset));
	
	FVector velocity = FVector(initialVelocity * UKismetMathLibrary::Cos(angle), 0, initialVelocity * UKismetMathLibrary::Sin(angle));

	// Rotate our velocity to match the direction between two objects
	float angleBetweenObjects = UKismetMathLibrary::RadiansToDegrees ((planarTarget - planarPosition).HeadingAngle());
	
	FVector finalVelocity = velocity.RotateAngleAxis(angleBetweenObjects, FVector::UpVector);
	
	return finalVelocity;
}

void ANavLinkProxy_Jump::OnSmartLinkReachedHelper(AActor* MovingActor, const FVector& DestinationPoint)
{ 
	ABlindEyeBaseCharacter* BaseCharacter = Cast<ABlindEyeBaseCharacter>(MovingActor);
	if (BaseCharacter == nullptr) return;

	FVector LaunchAngle = DestinationPoint - MovingActor->GetActorLocation();
	float AlteredJumpAngle = LaunchAngle.Rotation().Pitch + 45;
	FVector LaunchForce = CalculateDownwardVectorImpulse(BaseCharacter, DestinationPoint, AlteredJumpAngle);
	BaseCharacter->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Falling;
	BaseCharacter->GetCharacterMovement()->Velocity = LaunchForce;
}
