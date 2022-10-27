// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRush.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ACrowRush::ACrowRush()
{
	AbilityStates.Add(new FAimingStartState(this));
	AbilityType = EAbilityTypes::Unique1;
}

void ACrowRush::UpdatePlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		StartingPosition = BlindEyePlayer->GetActorLocation();
	}
}

void ACrowRush::ResetPlayerSpeed()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->MULT_ResetWalkMovementToNormal();

		FVector EndLocation = BlindEyePlayer->GetActorLocation();

		TArray<FHitResult> OutHits;
		if (UKismetSystemLibrary::SphereTraceMultiForObjects(World, StartingPosition, EndLocation, PullSphereRadius, EnemyObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true))
		{
			for (FHitResult OutHit : OutHits)
			{ 
				// calculate knockBack force from center of dash
				FVector ClosestPoint = UKismetMathLibrary::FindClosestPointOnLine(OutHit.Location, StartingPosition, EndLocation - StartingPosition);
				float distToCenter = FVector::Distance(ClosestPoint, OutHit.Location);
				float KnockBackToCenterPercent = distToCenter / PullSphereRadius;
				FVector VecToCenter = ClosestPoint - OutHit.Location;
				VecToCenter.Normalize();
				VecToCenter *= UKismetMathLibrary::Lerp(MinKnockTowardsCenterForce, MaxKnockTowardsCenterForce, KnockBackToCenterPercent);
				VecToCenter += FVector::UpVector * UKismetMathLibrary::Lerp(MinKnockUpToCenterForce, MaxKnockUpToCenterForce, KnockBackToCenterPercent);
				
				// Calculate knock force to end of dash
				float distToEndOfRush = FVector::Distance(EndLocation, OutHit.Location);
				float KnockBackToEndPercent = distToEndOfRush / FVector::Distance(StartingPosition, EndLocation);
				FVector VecToEnd = EndLocation - OutHit.Location;
				VecToEnd.Normalize();
				VecToEnd *= UKismetMathLibrary::Lerp(MinKnockUpToEndForce, MaxKnockTowardsEndForce, KnockBackToEndPercent);
				VecToEnd += FVector::UpVector * UKismetMathLibrary::Lerp(MinKnockUpToEndForce, MaxKnockUpToEndForce, KnockBackToEndPercent);

				FVector ForceVecToApply = VecToEnd + VecToCenter;

				UGameplayStatics::ApplyPointDamage(OutHit.Actor.Get(), DamageAmount, OutHit.Location, OutHit, GetInstigatorController(),
					GetInstigator(), DamageType);

				// prevent knockback on other player
				if (Cast<ABlindEyePlayerCharacter>(OutHit.Actor))
				{
					continue;
				}

				if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OutHit.Actor))
				{
					if (UHealthComponent* HealthComponent = HealthInterface->GetHealthComponent())
					{
						HealthComponent->KnockBack(ForceVecToApply, GetInstigator());
					}
				}
			}
		}
	}
}

void ACrowRush::StartAiming()
{
	UWorld* World = GetWorld();
	if (World)
	{
		Target = World->SpawnActor<ACrowRushTarget>(TargetType);

		World->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ACrowRush::UpdateTargetPosition, UpdateTargetDelay, true);
	}
}

void ACrowRush::UpdateTargetPosition()
{
	Target->SetActorLocation(CalculateTargetPosition());
}

FVector ACrowRush::CalculateTargetPosition()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		GetInstigator()->GetController()->GetPlayerViewPoint(ViewportLocation, ViewportRotation);

		FVector EndLocation = ViewportLocation + ViewportRotation.Vector() * MaxDistance;

		FVector TargetPosition;
		FHitResult Hit;
		// if Hit point
		if (UKismetSystemLibrary::LineTraceSingleForObjects(World, ViewportLocation, EndLocation, TargetObjectBlocker,
			false, TArray<AActor*>(), EDrawDebugTrace::None, Hit, true))
		{
			return Hit.Location;
		}
		// Otherwise, set to furthest endpoint
		else
		{
			return EndLocation;
		}
	}
	return FVector::ZeroVector;
}

void ACrowRush::StartMovement()
{
	Target->Destroy();
}

// **** States *******

// Start Aiming state ******************

FAimingStartState::FAimingStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FAimingStartState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		//Ability->BP_AbilityStarted();
		RunState();
	}
}

void FAimingStartState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	// Start Aiming hold
	if (abilityUsageType == EAbilityInputTypes::None)
	{
		ACrowRush* Rush = Cast<ACrowRush>(Ability);
		check(Rush)
		Rush->StartAiming();
	}
	// stop aiming and goto moving state
	else if (abilityUsageType == EAbilityInputTypes::Released)
	{
		ExitState();
	}
}

void FAimingStartState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

// Moving to target state ******************

FMovingState::FMovingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FMovingState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
}

void FMovingState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
}

void FMovingState::ExitState()
{
	FAbilityState::ExitState();
}

// Ending State ******************

FEndState::FEndState(AAbilityBase* ability) : FAbilityState(ability) {}

void FEndState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
}

void FEndState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
}

void FEndState::ExitState()
{
	FAbilityState::ExitState();
}
