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
	
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	CachedGravityScale = Character->GetCharacterMovement()->GravityScale;
}

void APhoenixDive::LaunchPlayerUpwards()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * LaunchUpForcePower);
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().SetTimer(MaxHangingTimerHandle, this, &APhoenixDive::hangingInAirExpired, MaxTimeHanging, false);
}

void APhoenixDive::HangInAir()
{
	BP_AbilityInnerState(1);
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 0.f;
	Character->GetCharacterMovement()->StopMovementImmediately();

	// Ground Target
	CLI_SpawnGroundTarget();
	
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::HangInAirTimer()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(HangInAirTimerHandle, this, &APhoenixDive::HangInAir, DurationOfUpwardsForce, false);
}

void APhoenixDive::LaunchToGround()
{
	UWorld* world = GetWorld();
	if (!world) return;

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
		FVector ViewportLocation;
		FRotator ViewportRotation;
		CalculateLaunchViewPoint(ViewportLocation, ViewportRotation);

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
	
	Character->GetCharacterMovement()->Velocity = ImpulseVec;

	// prevent hanging in air
	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	// remove hanging expiration
	world->GetTimerManager().ClearTimer(MaxHangingTimerHandle);

	CLI_StopGroundTarget();
	// Setup ground collision
	Character->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APhoenixDive::CollisionWithGround);
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
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_PlayAnimMontage(DiveAbilityAnim);
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void APhoenixDive::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::PlayLandingSectionOfAnim()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->MULT_SetNextMontageSection(DiveAbilityAnim, "Land"); 
	}
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void APhoenixDive::LandingAnimationFinishExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::UpdateGroundTargetPosition()
{
	if (GroundTarget == nullptr) return;
	FVector targetPosition;
	if (CalculateGroundTargetPosition(targetPosition))
	{
		GroundTarget->SetActorLocation(targetPosition);
	}
}

bool APhoenixDive::CalculateGroundTargetPosition(FVector& TargetPosition)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;
	
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (Character == nullptr) return false;

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
	return false;
}

void APhoenixDive::CLI_SpawnGroundTarget_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;
	GroundTarget = world->SpawnActor<AGroundTarget>(GroundTargetType);
	world->GetTimerManager().SetTimer(UpdateGroundTargetPositionTimerHandle, this, &APhoenixDive::UpdateGroundTargetPosition, 0.02, true);
}

void APhoenixDive::CLI_StopGroundTarget_Implementation()
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

void APhoenixDive::hangingInAirExpired()
{
	// force a user input to launch to ground after time expired
	AbilityStates[CurrState]->TryEnterState(EAbilityInputTypes::Pressed);
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

void APhoenixDive::CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* world = GetWorld();
	if (!world) return;

	BP_AbilityInnerState(3);

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
}

void APhoenixDive::UnsubscribeToGroundCollision()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (!Character) return;
	
	// unbind delegate
	Character->GetCapsuleComponent()->OnComponentHit.Remove(this, TEXT("CollisionWithGround"));
	AbilityStates[CurrState]->ExitState();
}

void APhoenixDive::EndAbilityLogic()
{
	Super::EndAbilityLogic();
}

// **** States *******

// Start Ability State **************

FStartAbilityState::FStartAbilityState(AAbilityBase* ability) : FAbilityState(ability) {}

void FStartAbilityState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	if (!Ability) return;
	if (APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability))
	{
		if (!PhoenixDive->TryConsumeBirdMeter(PhoenixDive->CostPercent)) return;
	}
	RunState();
}

void FStartAbilityState::RunState(EAbilityInputTypes abilityUsageType)
{
	// prevent user input here
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType);

	if (Ability == nullptr) return;
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (PhoenixDive == nullptr) return;

	PhoenixDive->PlayAbilityAnimation();
}

void FStartAbilityState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;
	Ability->EndCurrState();
	// goto next state
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Jumping State *********************

FJumpState::FJumpState(AAbilityBase* ability) : FAbilityState(ability) {}

void FJumpState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	// prevent user input here
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FJumpState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState(abilityUsageType);
	if (!Ability) return;
	Ability->BP_AbilityStarted();
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

// In Air State *********************

FInAirState::FInAirState(AAbilityBase* ability) : FAbilityState(ability) {}

void FInAirState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FInAirState::RunState(EAbilityInputTypes abilityUsageType)
{
	if (abilityUsageType > EAbilityInputTypes::None) return;
	FAbilityState::RunState(abilityUsageType);
	
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

// Hanging State **********

FHangingState::FHangingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FHangingState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	// enter launch state on player input, or duration in air ran out
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FHangingState::RunState(EAbilityInputTypes abilityUsageType)
{
	if (abilityUsageType > EAbilityInputTypes::None) return;
	// Launch to ground
	FAbilityState::RunState(abilityUsageType);
	if (!Ability) return;
	APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability);
	if (!PhoenixDive) return;

	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;

	PhoenixDive->LaunchToGround();
	Ability->BP_AbilityInnerState(2);
}

void FHangingState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

// Colliding with ground state **********

FHitGroundState::FHitGroundState(AAbilityBase* ability) : FAbilityState(ability) {}

void FHitGroundState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	RunState();
}

void FHitGroundState::RunState(EAbilityInputTypes abilityUsageType)
{
	// prevent user input
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType);

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


