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
}

void APhoenixDive::LaunchPlayerUpwards()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * LaunchUpForcePower);

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
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 1.f;
	
	FVector ViewportLocation;
	FRotator ViewportRotation;
	CalculateLaunchViewPoint(ViewportLocation, ViewportRotation);

	Character->GetCharacterMovement()->AddImpulse(ViewportRotation.Vector() * LaunchDownForcePower);

	UWorld* world = GetWorld();
	if (!world) return;

	// prevent hanging in air
	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	// remove hanging expiration
	world->GetTimerManager().ClearTimer(MaxHangingTimerHandle);

	CLI_StopGroundTarget();

	Character->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APhoenixDive::CollisionWithGround);
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
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	if (Character == nullptr) return;

	FVector ViewportLocation;
	FRotator ViewportRotation;
	CalculateLaunchViewPoint(ViewportLocation, ViewportRotation);
	
	FVector EndLineCheck = ViewportLocation + ViewportRotation.Vector() * 10000;

	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(World, Character->GetActorLocation(), EndLineCheck, GroundObjectTypes,
		false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
	{
		GroundTarget->SetActorLocation(OutHit.Location);
	}
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
	if (!Ability) return;
	if (APhoenixDive* PhoenixDive = Cast<APhoenixDive>(Ability))
	{
		if (!PhoenixDive->TryConsumeBirdMeter(PhoenixDive->CostPercent)) return;
	}
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
}

// Hanging State **********

FHangingState::FHangingState(AAbilityBase* ability) : FAbilityState(ability) {}

void FHangingState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState(abilityUsageType);
	if (abilityUsageType == EAbilityInputTypes::Pressed)
	{
		RunState();
	}
}

void FHangingState::RunState(EAbilityInputTypes abilityUsageType)
{
	if (abilityUsageType > EAbilityInputTypes::None) return;
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


