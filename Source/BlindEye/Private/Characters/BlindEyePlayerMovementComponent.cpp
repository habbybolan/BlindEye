// Copyright (C) Nicholas Johnson 2022
#include "Characters/BlindEyePlayerMovementComponent.h"

#include "Characters/BlindEyePlayerController.h"
#include "GameFramework/Character.h"

void UBlindEyePlayerMovementComponent::PhysicsRotation(float DeltaTime)
{
	Super::PhysicsRotation(DeltaTime);

	if (bOrientRotationToMouse)
	{
		if (bOrientRotationToMovement || bUseControllerDesiredRotation)
		{
			UE_LOG(LogTemp, Warning, TEXT("Clear bOrientRotationToMovement and bUseControllerDesiredRotation before using bOrientRotationToMouse"))
			return;
		}

		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		FRotator DesiredRotation = CurrentRotation;
		if (CharacterOwner->Controller)
		{
			ABlindEyePlayerController* BlindEyePlayerController = Cast<ABlindEyePlayerController>(CharacterOwner->Controller);
			if (ensure(BlindEyePlayerController))
			{
				DesiredRotation = BlindEyePlayerController->GetDesiredRotationFromMouse();
			} else return;
		}
		else return;
	
		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
		{
			DesiredRotation.Pitch = 0;
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			DesiredRotation.Roll = 0;

			// Set the new rotation.
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
	}
}
