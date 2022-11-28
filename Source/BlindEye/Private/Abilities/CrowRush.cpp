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
	AbilityType = EAbilityTypes::Unique2;
}

void ACrowRush::ApplyDamage()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
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

void ACrowRush::CLI_StartAiming_Implementation()
{
	UWorld* World = GetWorld();
	if (World)
	{
		Target = World->SpawnActor<ACrowRushTarget>(TargetType);

		World->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ACrowRush::UpdateTargetPosition, UpdateMovementDelay, true);
	}
}

void ACrowRush::MULT_ResetPlayerState_Implementation()
{
	CurrDuration = 0;
	
	// reset player state
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Player->GetCharacterMovement()->PrimaryComponentTick.bCanEverTick = true;
}
 
void ACrowRush::UpdateTargetPosition()
{
	if (Target)
	{
		Target->SetActorLocation(CalculateTargetPosition());
	}
}

FVector ACrowRush::CalculateTargetPosition()
{
	FVector TargetPosition;
	UWorld* World = GetWorld();
	if (World)
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);

		FVector EndLocation = ViewportLocation + ViewportRotation.Vector() * MaxDistance;

		bool bValidLocation = false;
		FHitResult HitTargetLocation;
		// if Hit point
		if (UKismetSystemLibrary::LineTraceSingleForObjects(World, ViewportLocation, EndLocation, TargetObjectBlocker,
			false, TArray<AActor*>(), EDrawDebugTrace::None, HitTargetLocation, true))
		{
			TargetPosition = HitTargetLocation.Location;
			bValidLocation = true;
		}
		// Otherwise, set to furthest endpoint
		else
		{
			TargetPosition = EndLocation;
			FVector DirBackAlongLine = ViewportLocation - TargetPosition;
			DirBackAlongLine.Normalize();

			// Find point along line to target that has ground underneath
			if (FindValidTargetLocation(TargetPosition, ViewportLocation, DirBackAlongLine, 1))
			{
				bValidLocation = true;
			}
		}

		if (bValidLocation)
		{
			FHitResult HitTargetBelow;
			// Move target position off ground based on player height
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
			float PlayerHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			if (UKismetSystemLibrary::LineTraceSingleForObjects(World, TargetPosition, TargetPosition + FVector::DownVector * PlayerHeight, TargetObjectBlocker,
				false, TArray<AActor*>(), EDrawDebugTrace::None, HitTargetBelow, true))
			{
				// Move target based on the different between player height and how close it's to the ground
				float DistFromGround = FVector::Distance(HitTargetBelow.Location, TargetPosition);
				TargetPosition += FVector::UpVector * FMath::Abs(DistFromGround - PlayerHeight) + FVector::UpVector * TargetPositionOffset;
			}
		} else
		{
			// No valid location, just teleport to max distance
			TargetPosition = EndLocation;
		}
	}
	return TargetPosition;
}
 
bool ACrowRush::FindValidTargetLocation(FVector& CurrLoc, FVector EndLoc, FVector DirBackAlongLine, uint8 StepNum)
{ 
	if (StepNum * StepSize > MaxDistance) return false;

	// TODO: Incr. max distance by amount (when aiming straight up)
	FHitResult HitTargetBelow;
	// If valid ground below, then set as target point
	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CurrLoc, CurrLoc + FVector::DownVector * MaxDistance, TargetObjectBlocker,
			false, TArray<AActor*>(), EDrawDebugTrace::None, HitTargetBelow, true))
	{
		// Apply step offset if still on island to prevent being on very edge
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CurrLoc + DirBackAlongLine * StepOffsetTowardsPlayer,
			CurrLoc + DirBackAlongLine * StepOffsetTowardsPlayer + FVector::DownVector * MaxDistance, TargetObjectBlocker,
			false, TArray<AActor*>(), EDrawDebugTrace::None, HitTargetBelow, true))
		{
			CurrLoc += DirBackAlongLine * StepOffsetTowardsPlayer;
		}
		return true;
	}
	// Otherwise, continue stepping along line
	else
	{
		CurrLoc += DirBackAlongLine * StepNum * StepSize;
		return FindValidTargetLocation(CurrLoc, EndLoc, DirBackAlongLine, ++StepNum);
	}
}

void ACrowRush::EndAbilityLogic()
{
	Super::EndAbilityLogic();

	MULT_ResetPlayerState();
}

void ACrowRush::StartMovement()
{
	FVector CalcEndPos = CalculateTargetPosition();
	FVector StartPos = GetInstigator()->GetActorLocation();
	CLI_RemoveTarget();
	MULT_StartMovementHelper(StartPos, CalcEndPos);
}
 
 
void ACrowRush::MULT_StartMovementHelper_Implementation(FVector StartPos, FVector CalculatedEndPos) 
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->GetMovementComponent()->StopMovementImmediately();
	
	UWorld* World = GetWorld();
	if (World)
	{ 
		StartingPosition = StartPos;
		EndPosition = CalculatedEndPos;
		CalculatedDuration = (FVector::Distance(StartingPosition, EndPosition) / MaxDistance) * DurationAtMaxDistance;

		GetInstigator()->SetActorLocation(EndPosition);

		if (GetLocalRole() == ROLE_Authority)
		{
			ApplyDamage();

			// If Already on the ground, then dont play landing montage
			if (CheckIsLandedHelper())
			{
				SetAsLanded();
			}
			// Otherwise, wait to land to play anim
			else
			{
				World->GetTimerManager().SetTimer(CheckIsLandedTimerHandle, this, &ACrowRush::CheckIsLanded, 0.1, true);
			}
		}

		// ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
		// Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// Player->GetCharacterMovement()->PrimaryComponentTick.bCanEverTick = false;
		// Player->GetCharacterMovement()->Deactivate();
		//
		// World->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ACrowRush::UpdatePlayerMovement, UpdateMovementDelay, true);
	}
}

void ACrowRush::CheckIsLanded()
{
	UWorld* World = GetWorld();
	if (World && CheckIsLandedHelper())
	{
		World->GetTimerManager().ClearTimer(CheckIsLandedTimerHandle);
		SetAsLanded();
	}
}

bool ACrowRush::CheckIsLandedHelper()
{
	UWorld* World = GetWorld();
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	if (World)
	{
		FVector StartLoc = Player->GetMesh()->GetBoneLocation("RightFoot");
		FVector EndLoc = StartLoc + FVector::DownVector * 150;
		FHitResult OutHit;
		return UKismetSystemLibrary::LineTraceSingleForObjects(World, StartLoc, EndLoc, GroundObjectTypes, false,
			TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHit, true);
	}
	return false;
}

void ACrowRush::SetAsLanded()
{
	BP_AbilityInnerState(2);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->MULT_PlayAnimMontage(LandingAnim);
	if (!Player->GetMesh()->GetAnimInstance()->OnMontageEnded.Contains(this, TEXT("SetLandingAnimFinished")))
	{
		Player->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACrowRush::SetLandingAnimFinished);
	}
}

void ACrowRush::SetLandingAnimFinished(UAnimMontage* Montage, bool bInterrupted)
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->GetMesh()->GetAnimInstance()->OnMontageEnded.Remove(this, TEXT("SetLandingAnimFinished"));
	// Landing finished, goto ending state
	if (GetLocalRole() == ROLE_Authority)
	{
		AbilityStates[CurrState]->ExitState();
	}
}

void ACrowRush::CLI_RemoveTarget_Implementation()
{
	if (Target)
	{
		Target->Destroy();
	}
} 

void ACrowRush::UpdatePlayerMovement()
{
	FVector Ease = UKismetMathLibrary::VEase(StartingPosition, EndPosition, CurrDuration / CalculatedDuration, EasingFunction);
	GetInstigator()->SetActorLocation(Ease);
	CurrDuration += UpdateMovementDelay;

	// If finished movement
	if (CurrDuration >= CalculatedDuration)
	{
		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().ClearTimer(UpdateTargetTimerHandle);

		if (GetLocalRole() == ROLE_Authority)
		{
			ApplyDamage();
			AbilityStates[CurrState]->ExitState();
		}
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
		Ability->AbilityStarted();
		ACrowRush* Rush = Cast<ACrowRush>(Ability);
		check(Rush)
		Rush->CLI_StartAiming();
	}
	// stop aiming and goto moving state
	else if (abilityUsageType == EAbilityInputTypes::Released)
	{
		Ability->BP_AbilityInnerState(1);
		ExitState();
	}
}

void FAimingStartState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FAimingStartState::CancelState()
{
	FAbilityState::CancelState();
	
	ACrowRush* Rush = Cast<ACrowRush>(Ability);
	Rush->CLI_RemoveTarget();
	// resets camera and stops particles
	Rush->BP_AbilityInnerState(2);

	UWorld* World = Rush->GetWorld();
	{
		World->GetTimerManager().ClearTimer(Rush->UpdateTargetTimerHandle);
	}
	return true;
}

// Moving to target state ******************

FMovingState::FMovingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FMovingState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	if (!Ability) return;
	Ability->SetOnCooldown();
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
}

bool FMovingState::CancelState()
{
	FAbilityState::CancelState();

	ACrowRush* Rush = Cast<ACrowRush>(Ability);
	// resets camera and stops particles
	Rush->BP_AbilityInnerState(2);
	UWorld* World = Rush->GetWorld();
	{
		World->GetTimerManager().ClearTimer(Rush->CheckIsLandedTimerHandle);
	}
	return true;
}
