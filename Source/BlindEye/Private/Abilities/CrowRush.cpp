// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRush.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ACrowRush::ACrowRush()
{
	AbilityStates.Add(new FCrowRushStartState(this));
}

void ACrowRush::UpdatePlayerSpeed()
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		StartingPosition = BlindEyePlayer->GetActorLocation();
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600 * DashSpeedIncrease;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048 * DashAccelerationIncrease;
	}
}

void ACrowRush::ResetPlayerSpeed()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyePlayer->GetCharacterMovement()->MaxWalkSpeed = 600;
		BlindEyePlayer->GetCharacterMovement()->MaxAcceleration = 2048;

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

// **** States *******

// Dash Start State *********************

FCrowRushStartState::FCrowRushStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FCrowRushStartState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FCrowRushStartState::RunState(EAbilityInputTypes abilityUsageType)
{
	// prevent user input
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType);
	if (Ability == nullptr) return;

	Ability->BP_AbilityStarted();
	ACrowRush* Dash = Cast<ACrowRush>(Ability);
	if (Dash == nullptr) return;

	Dash->UpdatePlayerSpeed();
	Dash->DelayToNextState(Dash->DashDuration, true);
}

void FCrowRushStartState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;

	ACrowRush* Dash = Cast<ACrowRush>(Ability);
	if (Dash == nullptr) return;

	Dash->ResetPlayerSpeed();
	Ability->EndCurrState();
}
