// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixDive.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APhoenixDive::APhoenixDive() : AAbilityBase()
{
	AbilityStates.Add(new FJumpState(this));
	AbilityStates.Add(new FInAirState(this));
	AbilityStates.Add(new FHangingState(this));
}

void APhoenixDive::LaunchPlayerUpwards()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->AddImpulse(FVector::UpVector * 100000);

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().SetTimer(MaxHangingTimerHandle, this, &APhoenixDive::hangingInAirExpired, MaxTimeHanging, false);
}

void APhoenixDive::HangInAir()
{
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

	world->GetTimerManager().SetTimer(HangInAirTimerHandle, this, &APhoenixDive::HangInAir, 1.0f, false);
}

void APhoenixDive::LaunchToGround()
{
	ACharacter* Character = Cast<ACharacter>(GetInstigator());
	Character->GetCharacterMovement()->GravityScale = 1.f;

	FVector LaunchVector = GroundTarget->GetActorLocation() - Character->GetActorLocation();
	LaunchVector.Normalize();
	Character->GetCharacterMovement()->AddImpulse(LaunchVector * 200000);

	UWorld* world = GetWorld();
	if (!world) return;

	// prevent hanging in air
	world->GetTimerManager().ClearTimer(HangInAirTimerHandle);
	// remove hanging expiration
	world->GetTimerManager().ClearTimer(MaxHangingTimerHandle);

	CLI_StopGroundTarget();

	Character->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APhoenixDive::CollisionWithGround);
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
	GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);

	ViewportRotation.Pitch = FMath::ClampAngle(ViewportRotation.Pitch, -90, -90 + ClampPitchDegrees);
	
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

	BP_AbilityInnerState(2);

	// Apply damage to self for detonation effect
	UGameplayStatics::ApplyPointDamage(GetInstigator(), Damage, FVector::ZeroVector, FHitResult(),
		GetInstigator()->GetController(), GetInstigator(), DamageType);
	// Apply damage to rest of enemies
	UGameplayStatics::ApplyRadialDamage(world, Damage, Hit.Location, Radius, DamageType, TArray<AActor*>(), GetInstigator());
	
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

// Jumping State *********************

FJumpState::FJumpState(AAbilityBase* ability) : FAbilityState(ability) {}

void FJumpState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
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
	
	PhoenixDive->LaunchToGround();
	Ability->BP_AbilityInnerState(1);
}

void FHangingState::ExitState()
{
	FAbilityState::ExitState();
	if (!Ability) return;
	Ability->EndCurrState();
}


