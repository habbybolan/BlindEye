// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixDive.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APhoenixDive::APhoenixDive() : AAbilityBase()
{
	AbilityStates.Add(new FStartAbilityState(this));
	AbilityStates.Add(new FJumpState(this));
	AbilityStates.Add(new FInAirState(this));
	AbilityStates.Add(new FHangingState(this));
	AbilityStates.Add(new FHitGroundState(this));
	AbilityType = EAbilityTypes::Unique2;
}

void APhoenixDive::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(GetInstigator())) return;
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	CachedGravityScale = Character->GetCharacterMovement()->GravityScale;
}

void APhoenixDive::LaunchPlayerUpwards()
{
	LaunchPlayerUpwardsHelper();
	MULT_LaunchPlayerUpwards();
	
	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().SetTimer(MaxHangingTimerHandle, this, &APhoenixDive::hangingInAirExpired, MaxTimeHanging, false);
}

void APhoenixDive::MULT_LaunchPlayerUpwards_Implementation()
{
	// Run on remote clients
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		LaunchPlayerUpwardsHelper();
	}
}

void APhoenixDive::LaunchPlayerUpwardsHelper()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * LaunchUpForcePower);
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void APhoenixDive::hangingInAirExpired()
{
	// force a user input to launch to ground after time expired
	AbilityStates[CurrState]->TryEnterState(EAbilityInputTypes::Pressed);
}

void APhoenixDive::HangInAirTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(HangInAirTimerHandle, this, &APhoenixDive::HangInAir, DurationOfUpwardsForce, false);
}

void APhoenixDive::HangInAir()
{
	AbilityInnerState(1);

	HandInAirHelper();
	MULT_HandInAirHelper();
	
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
	check(Player)
	if (Player->IsLocallyControlled())
	{
		SpawnGroundTarget();
	}
	
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::HandInAirHelper()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 0.f;
	Character->GetCharacterMovement()->StopMovementImmediately();
}

void APhoenixDive::MULT_HandInAirHelper_Implementation()
{
	// Run on remote clients
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		HandInAirHelper();
	}
}

void APhoenixDive::SpawnGroundTarget()
{
	UWorld* world = GetWorld();
	if (!world) return;
	GroundTarget = world->SpawnActor<AGroundTarget>(GroundTargetType);
	world->GetTimerManager().SetTimer(UpdateGroundTargetPositionTimerHandle, this, &APhoenixDive::UpdateGroundTargetPosition, 0.02, true);
}

void APhoenixDive::UpdateGroundTargetPosition()
{
	if (GroundTarget == nullptr) return;
	FVector targetPosition;
	if (CalculateGroundTargetPosition(targetPosition))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Emerald, targetPosition.ToString());
		GroundTarget->SetActorLocation(targetPosition);
	}
}

void APhoenixDive::LaunchToGround()
{
	UWorld* world = GetWorld();
	if (!world) return;
 
	SetOnCooldown(); 

	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (Character == nullptr) return;

	Character->GetCharacterMovement()->GravityScale = CachedGravityScale;
	
	FVector position;	// Position of ground target
	FVector ImpulseVec;	// Force vector to apply to player
	// if something happened to ground target, launch straight down
	if (!CalculateGroundTargetPosition(position))
	{
		ImpulseVec = FVector(0, 0, -MinDownwardForceCanApply);
	}
	// Otherwise launch normally
	else
	{
		FVector VecToGroundTarget = position - GetOwner()->GetActorLocation();
		FRotator RotatorToGroundTarget = VecToGroundTarget.Rotation();
		float Angle = RotatorToGroundTarget.Pitch;
 
		// Launch player towards ground target location
		ImpulseVec = CalculateDownwardVectorImpulse( position, Angle + AngleUpOffsetOnLaunch);
	
		// Prevent small amount of force being applied when aiming too low down
		if (ImpulseVec.Size() < MinDownwardForceCanApply)
		{
			ImpulseVec *= 1 + (MinDownwardForceCanApply - ImpulseVec.Size()) / ImpulseVec.Size();
		}
	}

	LaunchToGroundHelper(ImpulseVec);
	MULT_LaunchToGround_Implementation(ImpulseVec);
	
	// prevent hanging in air
	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	// remove hanging expiration
	world->GetTimerManager().ClearTimer(MaxHangingTimerHandle);
	
	// Setup ground collision
	Character->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APhoenixDive::CollisionWithGround);

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		StopGroundTarget();
	}
}

void APhoenixDive::MULT_LaunchToGround_Implementation(FVector LaunchForce)
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		LaunchToGroundHelper(LaunchForce);
	}
}

void APhoenixDive::LaunchToGroundHelper(FVector LaunchForce)
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (Character == nullptr) return;

	Character->GetCharacterMovement()->GravityScale = CachedGravityScale;
	Character->GetCharacterMovement()->Velocity = LaunchForce;
}

void APhoenixDive::CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* world = GetWorld();
	if (!world) return;

	AbilityInnerState(3);

	// Apply damage to self for detonation effect
	UGameplayStatics::ApplyPointDamage(GetInstigator(), Damage, FVector::ZeroVector, FHitResult(),
		GetInstigator()->GetController(), GetInstigator(), DamageType);

	// Apply damage to rest of enemies
	TArray<FHitResult> OutHits;
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(world, Hit.Location, Hit.Location + FVector::UpVector * -10, Radius, EnemyObjectTypes,
		false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true))
	{
		for (FHitResult CollisionHit : OutHits)
		{
			UGameplayStatics::ApplyPointDamage(CollisionHit.Actor.Get(), Damage, CollisionHit.Location, CollisionHit, GetInstigator()->GetController(),
				GetInstigator(), DamageType);
		}
	}
	UnsubscribeToGroundCollision();

	CollisionWithGroundHelper();
	MULT_CollisionWithGround();
}

void APhoenixDive::CollisionWithGroundHelper()
{
	ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	PlayerCharacter->GetCharacterMovement()->GravityScale = CachedGravityScale;
}

void APhoenixDive::MULT_CollisionWithGround_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		CollisionWithGroundHelper();
	}
}

FVector APhoenixDive::CalculateDownwardVectorImpulse(FVector TargetPosition, float Angle)
{
	float angle = UKismetMathLibrary::DegreesToRadians(Angle);

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	// position of this object and the target on the same plane
	FVector planarTarget = FVector(TargetPosition.X, TargetPosition.Y, 0);
	FVector planarPosition = FVector(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y, 0);

	// Planar distance between objects
	float distance = FVector::Distance(planarTarget, planarPosition);
	// distance along the y axis between objects
	float ZOffset = GetOwner()->GetActorLocation().Z - TargetPosition.Z;

	float initialVelocity = (1 / UKismetMathLibrary::Cos(angle)) *
		UKismetMathLibrary::Sqrt(
			(0.5f * -1 * Character->GetCharacterMovement()->GetGravityZ() * distance * distance)
			/ (distance * UKismetMathLibrary::Tan(angle) + ZOffset));
	
	FVector velocity = FVector(initialVelocity * UKismetMathLibrary::Cos(angle), 0, initialVelocity * UKismetMathLibrary::Sin(angle));

	// Rotate our velocity to match the direction between two objects
	float angleBetweenObjects = UKismetMathLibrary::RadiansToDegrees ((planarTarget - planarPosition).HeadingAngle());
	
	FVector finalVelocity = velocity.RotateAngleAxis(angleBetweenObjects, FVector::UpVector);
	
	return finalVelocity;
}

void APhoenixDive::PlayAbilityAnimation()
{
	PlayAbilityAnimationHelper();
	AnimNotifyDelegate.BindDynamic( this, &APhoenixDive::UseAnimNotifyExecuted);
}

void APhoenixDive::MULT_PlayAbilityAnimation_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		PlayAbilityAnimationHelper();
	}
}

void APhoenixDive::PlayAbilityAnimationHelper()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->PlayAnimMontage(DiveAbilityAnim);
	}
}

void APhoenixDive::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::PlayLandingSectionOfAnim() 
{
	PlayLandingSectionOfAnimHelper();
	MULT_PlayLandingSectionOfAnim();
	AnimNotifyDelegate.BindDynamic( this,  &APhoenixDive::UseAnimNotifyExecuted);
}

void APhoenixDive::PlayLandingSectionOfAnimHelper()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_JumpToSection("Land", DiveAbilityAnim); 
	}
}

void APhoenixDive::MULT_PlayLandingSectionOfAnim_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		PlayLandingSectionOfAnimHelper();
	}
}

void APhoenixDive::LandingAnimationFinishExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::MULT_ResetPlayerOnCancel_Implementation()
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ResetPlayerOnCancelHelper();
	}
}

void APhoenixDive::ResetPlayerOnCancelHelper()
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
	Player->GetCharacterMovement()->GravityScale = CachedGravityScale;
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

bool APhoenixDive::CalculateGroundTargetPosition(FVector& TargetPosition)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;
	
	ABlindEyePlayerCharacter* Character = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	if (Character == nullptr) return false;

	if (Character->GetIsTopdown())
	{
		return GetMouseTargetLocationHelper(OUT TargetPosition, TArray<TEnumAsByte<EObjectTypeQuery>>(), false);
	} else
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		CalculateLaunchViewPoint(ViewportLocation, ViewportRotation);

		FVector EndPosition = ViewportLocation + ViewportRotation.Vector() * 10000;
		FHitResult OutHit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(World, Character->GetActorLocation(), EndPosition, GroundObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
		{
			TargetPosition = OutHit.Location;
			return true;
		}	
	}
	
	return false;
}

void APhoenixDive::StopGroundTarget()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	world->GetTimerManager().ClearTimer(UpdateGroundTargetPositionTimerHandle);
	if (GroundTarget != nullptr)
	{
		GroundTarget->Destroy();
		GroundTarget = nullptr;
	}
}

FRotator APhoenixDive::CalculateLaunchViewPoint(FVector& ViewportLocation, FRotator& ViewportRotation)
{
	GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);
	ViewportRotation.Pitch = FMath::ClampAngle(ViewportRotation.Pitch, -90, -90 + ClampPitchDegrees);
	return ViewportRotation;
}

void APhoenixDive::EndLaunchUp()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	EndCurrState();
	// immediately enter new state
	UseAbility(EAbilityInputTypes::None);
}

void APhoenixDive::UnsubscribeToGroundCollision()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (!Character) return;
	
	// unbind delegate
	Character->GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &APhoenixDive::CollisionWithGround);
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::EndAbilityLogic()
{
	Super::EndAbilityLogic();

	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &APhoenixDive::CollisionWithGround);

	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = false;
		Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}
}

// **** States *******

// Start Ability State **************

FStartAbilityState::FStartAbilityState(AAbilityBase* ability) : FAbilityState(ability) {}

void FStartAbilityState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	if (APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability))
	{
		if (!PhoenixDive->TryConsumeBirdMeter(PhoenixDive->CostPercent)) return;
		if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PhoenixDive->GetOwner()))
		{
			Player->GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = true;
			Player->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
		}
	}
	RunState();
}

void FStartAbilityState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent user input here
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType, Location, Rotation);

	if (Ability == nullptr) return;
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (PhoenixDive == nullptr) return;

	PhoenixDive->PlayAbilityAnimation();
	PhoenixDive->MULT_PlayAbilityAnimation();
}

void FStartAbilityState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;
	Ability->EndCurrState();
	// goto next state
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FStartAbilityState::CancelState()
{
	FAbilityState::CancelState();

	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Ability->GetOwner());
	Player->MULT_StopAnimMontage(PhoenixDive->DiveAbilityAnim);
	return true;
}

// Jumping State *********************

FJumpState::FJumpState(AAbilityBase* ability) : FAbilityState(ability) {}

void FJumpState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent user input here
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	Ability->AbilityStarted();
	RunState();
}

void FJumpState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	PhoenixDive->LaunchPlayerUpwards();
	ExitState();
}

void FJumpState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;

	Ability->EndCurrState();
	// enter next state
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FJumpState::CancelState()
{
	FAbilityState::CancelState();
	return true;
}

// In Air State *********************

FInAirState::FInAirState(AAbilityBase* ability) : FAbilityState(ability) {}

void FInAirState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	RunState();
}

void FInAirState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	if (abilityUsageType > EAbilityInputTypes::None) return;
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	
	if (!Ability) return;
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;
	
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	PhoenixDive->HangInAirTimer(); 
} 

void FInAirState::ExitState() 
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FInAirState::CancelState()
{
	FAbilityState::CancelState(); 

	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	// clear timers for stopping at certain height and launching to ground
	UWorld* World = Ability->GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(PhoenixDive->MaxHangingTimerHandle);
		World->GetTimerManager().ClearTimer(PhoenixDive->HangInAirTimerHandle);
	}
	// Update player's state back to normal
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Ability->GetOwner());
	PhoenixDive->MULT_ResetPlayerOnCancel();
	PhoenixDive->ResetPlayerOnCancelHelper();
	Player->MULT_StopAnimMontage(PhoenixDive->DiveAbilityAnim);
	return true;
}

// Hanging State **********

FHangingState::FHangingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FHangingState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	// enter launch state on player input, or duration in air ran out
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FHangingState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent inputs in run state
	if (abilityUsageType > EAbilityInputTypes::None) return;
	// Launch to ground
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;

	// remove expiration timer
	if (UWorld* World = Ability->GetWorld())
	{
		World->GetTimerManager().ClearTimer(PhoenixDive->MaxHangingTimerHandle);
	}

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	PhoenixDive->LaunchToGround();
	Ability->AbilityInnerState(2);
}

void FHangingState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FHangingState::CancelState()
{
	// Hanging in air state
	if (CurrInnerState == EInnerState::None)
	{
		FAbilityState::CancelState();
		
		APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Ability->GetInstigator());
		PhoenixDive->MULT_ResetPlayerOnCancel();
		PhoenixDive->ResetPlayerOnCancelHelper();
		
		Player->MULT_StopAnimMontage(PhoenixDive->DiveAbilityAnim);
		if (Ability->GetOwner()->GetLocalRole() == ROLE_AutonomousProxy || Ability->GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
		{
			PhoenixDive->StopGroundTarget();
		}
		
		UWorld* World = Ability->GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(PhoenixDive->MaxHangingTimerHandle);
		}
		Ability->SetOnCooldown();
		return true;
	}
	// prevent cancellation when launching to ground
	return false;
}

// Colliding with ground state **********

FHitGroundState::FHitGroundState(AAbilityBase* ability) : FAbilityState(ability) {}

void FHitGroundState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	RunState();
}

void FHitGroundState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent user input
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
 
	if (!Ability) return;
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;
	PhoenixDive->PlayLandingSectionOfAnim();
}

void FHitGroundState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;
	Ability->EndCurrState();
}

bool FHitGroundState::CancelState()
{
	FAbilityState::CancelState();

	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PhoenixDive->GetOwner());
	Player->MULT_StopAnimMontage(PhoenixDive->DiveAbilityAnim);
	return true;
}


