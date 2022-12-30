// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRush.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ACrowRush::ACrowRush()
{
	AbilityStates.Add(new FAimingStartState(this));
	AbilityStates.Add(new FMovingState(this));
	AbilityStates.Add(new FLandingState(this));
	AbilityType = EAbilityTypes::Unique2;

	PrimaryActorTick.bCanEverTick = true;
}

void ACrowRush::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
}

void ACrowRush::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bMovementStarted)
	{
		LerpMovementCalculation();
	}
}

// *** Aiming start ***

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
	if (Target)
	{
		Target->SetActorLocation(CalculateTargetPosition());
	}
}

FVector ACrowRush::CalculateTargetPosition()
{
	FVector TargetPosition;
	UWorld* World = GetWorld();
	check(World)

	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());

	if (Player->GetIsTopdown())
	{
		GetMouseTargetLocationHelper(OUT TargetPosition, TArray<TEnumAsByte<EObjectTypeQuery>>(),false);
	} else
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

void ACrowRush::RemoveTarget()
{
	if (Target)
	{
		Target->Destroy();
	}
}

// *** Aiming end ***

// *** Movement Logic start ***

void ACrowRush::StartMovement()
{
	if (GetIsLocallyControlled())
	{
		RemoveTarget();
	}
	// cache starting, end and duration values
	CalcMovementValues();
	
	StartMovementHelper();
	MULT_StartMovementHelper(StartingPosition, EndPosition, CalculatedDuration);
}

void ACrowRush::CalcMovementValues()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	if (Player->GetIsTopdown())
	{
		StartingPosition = GetInstigator()->GetActorLocation();;
		ABlindEyePlayerController::GetMouseAimLocationHelper(OUT EndPosition, AimLocation, AimRotation, Player, World);
	} else
	{
		FVector CalcEndPos = CalculateTargetPosition();
		FVector StartPos = GetInstigator()->GetActorLocation();
		
		StartingPosition = StartPos;
		EndPosition = CalcEndPos;
	}

	CalculatedDuration = (FVector::Distance(StartingPosition, EndPosition) / MaxDistance) * DurationAtMaxDistance;
}

void ACrowRush::StartMovementHelper()
{
	UWorld* World = GetWorld();
	check(World);
	
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->PlayAnimMontage(MovementAnim);
	
	Player->GetMovementComponent()->StopMovementImmediately();
	Player->GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = true;
	Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
	Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Player->GetCapsuleComponent()->SetEnableGravity(false);

	bMovementStarted = true;
}

void ACrowRush::MULT_StartMovementHelper_Implementation(FVector startPos, FVector endPos, float duration) 
{
	// Play for remote clients
	if (!GetIsLocallyControlled() && GetInstigator()->GetLocalRole() != ROLE_Authority)
	{
		// cache movement values
		StartingPosition = startPos;
		EndPosition = endPos;
		CalculatedDuration = duration;

		// Start movement
		StartMovementHelper();
	}
}

void ACrowRush::LerpMovementCalculation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	float DeltaSeconds = World->DeltaTimeSeconds;
	
	FVector Ease = UKismetMathLibrary::VEase(StartingPosition, EndPosition, CurrDuration / CalculatedDuration, EasingFunction);
	GetInstigator()->SetActorLocation(Ease, false, nullptr, ETeleportType::ResetPhysics);
	CurrDuration += DeltaSeconds;

	FRotator TargetRotation = (EndPosition - StartingPosition).Rotation();
	float YawRot = FMath::FixedTurn(GetInstigator()->GetActorRotation().Yaw, TargetRotation.Yaw, DeltaRotationToTarget * DeltaSeconds);
	GetInstigator()->SetActorRotation(FRotator(0, YawRot, 0));
	
	// If finished movement
	if (CurrDuration >= CalculatedDuration)
	{
		ResetMovementState();
		bMovementStarted = false;
		if (GetIsLocallyControlled() || GetInstigator()->GetLocalRole() == ROLE_Authority)
		{
			OnMovementEnded();
		}
	}
}

void ACrowRush::OnMovementEnded()
{
	ApplyDamage();
	
	// If Already on the ground, then dont play landing montage
	if (CheckIsLandedHelper())
	{
		CheckIsLanded();
	}
	// Otherwise, wait to land to play anim
	else
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(CheckIsLandedTimerHandle, this, &ACrowRush::CheckIsLanded, 0.1, true);
		}
	}
}

void ACrowRush::ResetMovementState()
{
	CurrDuration = 0;
	ResetMovementStateHelper();
	MULT_ResetPlayerState();
}

void ACrowRush::ResetMovementStateHelper()
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = false;
	Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Player->GetCapsuleComponent()->SetEnableGravity(true);
}

void ACrowRush::MULT_ResetPlayerState_Implementation()
{
	if (!GetIsLocallyControlled() && GetInstigator()->GetLocalRole() != ROLE_Authority)
	{
		ResetMovementStateHelper();
	}
}

// *** Movement Logic End ***

// *** Landing Logic Start ***

void ACrowRush::CheckIsLanded()
{
	UWorld* World = GetWorld();
	if (World && CheckIsLandedHelper())
	{
		World->GetTimerManager().ClearTimer(CheckIsLandedTimerHandle);
		
		SetAsLanded();
		// play animation
		SetAsLandedHelper();
		MULT_SetAsLanded();
	}
}

bool ACrowRush::CheckIsLandedHelper()
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	return !Player->GetMovementComponent()->IsFalling();
}

void ACrowRush::SetAsLanded()
{
	AbilityInnerState(2);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	if (!Player->GetMesh()->GetAnimInstance()->OnMontageEnded.Contains(this, TEXT("SetLandingAnimFinished")))
	{
		Player->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACrowRush::SetLandingAnimFinished);
	}
	AbilityStates[CurrState]->ExitState();
}

void ACrowRush::SetAsLandedHelper()
{
	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator()))
	{
		Player->GetMesh()->GetAnimInstance()->Montage_JumpToSection("Landed");
	}
}

void ACrowRush::MULT_SetAsLanded_Implementation()
{
	// Play for remote clients
	if (!GetIsLocallyControlled() && GetInstigator()->GetLocalRole() != ROLE_Authority)
	{
		SetAsLandedHelper();
	}
}

void ACrowRush::SetLandingAnimFinished(UAnimMontage* Montage, bool bInterrupted)
{
	SetLandingAnimFinishedHelper();
	// Landing finished, goto ending state
	AbilityStates[CurrState]->ExitState();
}

void ACrowRush::SetLandingAnimFinishedHelper()
 {
 	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
 	Player->GetMesh()->GetAnimInstance()->OnMontageEnded.Remove(this, TEXT("SetLandingAnimFinished"));
 	// Called in case animation being cancelled
 	Player->GetMesh()->GetAnimInstance()->StopAllMontages(0);
 }

// *** Landing Logic End ***

void ACrowRush::ApplyDamage()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	if (GetOwner()->GetLocalRole() < ROLE_Authority) return;
	
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

void ACrowRush::EndAbilityLogic()
{
	Super::EndAbilityLogic();
	bMovementStarted = false;
	MULT_ResetPlayerState();
}

void ACrowRush::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACrowRush, CurrDuration, COND_SkipOwner);
}




// **** States *******

// Start Aiming state ******************

FAimingStartState::FAimingStartState(AAbilityBase* ability) : FAbilityState(ability) {}

void FAimingStartState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	// Enter on pressed
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		//Ability->BP_AbilityStarted();
		RunState();
	}
}

void FAimingStartState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	// Start Aiming hold
	if (abilityUsageType == EAbilityInputTypes::None)
	{
		Ability->AbilityStarted();
		ACrowRush* Rush = Cast<ACrowRush>(Ability);
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Ability->GetOwner());
		check(Player)
		if (Player->IsLocallyControlled())
		{
			Rush->StartAiming();
		}
	}
	// stop aiming and goto moving state
	else if (abilityUsageType == EAbilityInputTypes::Released)
	{
		Ability->AbilityInnerState(1);
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
	Rush->RemoveTarget();
	// resets camera and stops particles
	Rush->AbilityInnerState(2);

	UWorld* World = Rush->GetWorld();
	{
		World->GetTimerManager().ClearTimer(Rush->UpdateTargetTimerHandle);
	}
	return true;
}

// Moving to target state ******************

FMovingState::FMovingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FMovingState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	Ability->SetOnCooldown();
	RunState();
}

void FMovingState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent user input
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType, Location, Rotation);

	Ability->Blockers.IsMovementBlocked = true;
	ACrowRush* CrowRush = Cast<ACrowRush>(Ability);
	CrowRush->StartMovement();
}

void FMovingState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FMovingState::CancelState()
{
	FAbilityState::CancelState();

	ACrowRush* Rush = Cast<ACrowRush>(Ability);
	// resets camera and stops particles
	Rush->AbilityInnerState(2);
	UWorld* World = Rush->GetWorld();
	{
		World->GetTimerManager().ClearTimer(Rush->CheckIsLandedTimerHandle);
	}
	Rush->ResetMovementState();
	return true;
}

// Landing state ******************

FLandingState::FLandingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FLandingState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location,
	const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	RunState();
}

void FLandingState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	Ability->Blockers.IsMovementBlocked = true;
}

void FLandingState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

bool FLandingState::CancelState()
{
	FAbilityState::CancelState();
	ACrowRush* Rush = Cast<ACrowRush>(Ability);
	Rush->SetLandingAnimFinishedHelper();
	return true;
}
