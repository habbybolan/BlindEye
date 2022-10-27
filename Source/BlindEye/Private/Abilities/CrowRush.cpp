// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRush.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ACrowRush::ACrowRush()
{
	AbilityStates.Add(new FAimingStartState(this));
	AbilityStates.Add(new FMovingState(this));
	AbilityStates.Add(new FEndState(this));
	AbilityType = EAbilityTypes::Unique1;
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

		World->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ACrowRush::UpdateTargetPosition, UpdateMovementDelay, true);
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

void ACrowRush::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	CurrDuration = 0;

	// reset player state
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Falling;
	Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACrowRush::StartMovement()
{
	Target->Destroy();
	UWorld* World = GetWorld();
	if (World)
	{
		StartingPosition = GetInstigator()->GetActorLocation();
		EndPosition = CalculateTargetPosition();
		CalculatedDuration = (FVector::Distance(StartingPosition, EndPosition) / MaxDistance) * DurationAtMaxDistance;
		
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
		Player->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
		Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		World->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ACrowRush::UpdatePlayerMovement, UpdateMovementDelay, true);
	}
}

void ACrowRush::UpdatePlayerMovement()
{
	FVector Ease = UKismetMathLibrary::VEase(StartingPosition, EndPosition, CurrDuration / CalculatedDuration, EasingFunction);
	GetInstigator()->SetActorLocation(Ease);
	CurrDuration += UpdateMovementDelay;

	if (CurrDuration >= CalculatedDuration)
	{
		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().ClearTimer(UpdateTargetTimerHandle);
		AbilityStates[CurrState]->ExitState();
	}
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
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Moving to target state ******************

FMovingState::FMovingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FMovingState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FMovingState::RunState(EAbilityInputTypes abilityUsageType)
{
	// prevent user input
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType);

	Ability->Blockers.IsMovementBlocked = true;
	ACrowRush* CrowRush = Cast<ACrowRush>(Ability);
	check(CrowRush);
	CrowRush->StartMovement();
}

void FMovingState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Ending State ******************

FEndState::FEndState(AAbilityBase* ability) : FAbilityState(ability) {}

void FEndState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FEndState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	ExitState();
}

void FEndState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}
