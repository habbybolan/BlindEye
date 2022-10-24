// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemy.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Enemies/Hunter/HunterHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AHunterEnemy::AHunterEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHunterHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
}

void AHunterEnemy::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().SetTimer(ChargedAttackCooldownTimerHandle, this, &AHunterEnemy::SetChargedAttackOffCooldown, ChargedAttackCooldown / 2, false);
}

void AHunterEnemy::PerformJumpAttack()
{
	// TODO: Probably remove this
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			FVector Direction = Target->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			Direction += FVector::UpVector * JumpUpForce;
			GetCharacterMovement()->AddImpulse(Direction * ForceApplied);
			GetWorldTimerManager().SetTimer(JumpAttackSwingDelayTimerHandle, this, &AHunterEnemy::JumpAttackSwing, JumpAttackSwingDelay, false);
		}
	}
}

void AHunterEnemy::PerformChargedAttack()
{
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			bAttacking = true;
			bChargeAttackCooldown = true;
			ChargedAttackTargetLocation = Target->GetActorLocation();
			ChargedAttackStartLocation = GetActorLocation();
			//FVector Direction = Target->GetActorLocation() - GetActorLocation();
			// Direction.Normalize();
			// Direction += FVector::UpVector * JumpUpForce;
			// GetCharacterMovement()->AddImpulse(Direction * ForceApplied);
			// GetWorldTimerManager().SetTimer(JumpAttackSwingDelayTimerHandle, this, &AHunterEnemy::JumpAttackSwing, JumpAttackSwingDelay, false);
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetWorldTimerManager().SetTimer(PerformingChargedAttackTimerHandle, this, &AHunterEnemy::PerformingJumpAttack, 0.02, true);
			GetWorldTimerManager().SetTimer(ChargedAttackCooldownTimerHandle, this, &AHunterEnemy::SetChargedAttackOffCooldown, ChargedAttackCooldown, false);
		}
	}
}

void AHunterEnemy::PerformingJumpAttack()
{ 
	FVector ForwardEase = UKismetMathLibrary::VEase(ChargedAttackStartLocation * FVector(1, 1, 0),
		ChargedAttackTargetLocation* FVector(1, 1, 0), CurrTimeOfChargedAttack / ChargedAttackDuration, EEasingFunc::Linear);

	FVector UpEase;
	FVector HalfDirectionToTarget = (ChargedAttackTargetLocation - ChargedAttackStartLocation) / 2 + FVector::UpVector * 200;
	float HalfChargedAttackDuration = ChargedAttackDuration / 2;
	// If at first half of jump, jump from starting Z to jump Z-Peak
	if (CurrTimeOfChargedAttack / ChargedAttackDuration <= 0.5)
	{
		 UpEase = UKismetMathLibrary::VEase(ChargedAttackStartLocation * FVector::UpVector,
			(ChargedAttackTargetLocation + HalfDirectionToTarget) * FVector::UpVector, CurrTimeOfChargedAttack / HalfChargedAttackDuration, EEasingFunc::CircularOut);
	}
	// Other latter half of jump, Go from Jump Z-Peak to end point Z
	else
	{
		UpEase = UKismetMathLibrary::VEase((ChargedAttackTargetLocation + HalfDirectionToTarget) * FVector::UpVector,
		   ChargedAttackTargetLocation * FVector::UpVector,
		   (CurrTimeOfChargedAttack - HalfChargedAttackDuration) / (ChargedAttackDuration - HalfChargedAttackDuration), EEasingFunc::CircularIn);
	}

	SetActorLocation(ForwardEase + UpEase);
	CurrTimeOfChargedAttack += 0.02;

	if (CurrTimeOfChargedAttack >= ChargedAttackDuration)
	{
		GetWorldTimerManager().ClearTimer(PerformingChargedAttackTimerHandle);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bAttacking = false;
	}
}  

void AHunterEnemy::JumpAttackSwing()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 1000, FVector(0, 100 / 2, 100 / 2),
		GetActorRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);

	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, JumpAttackDamageType);
	}
}

void AHunterEnemy::TrySetVisibility(bool visibility)
{
	if (IsVisible == visibility) return;

	IsVisible = visibility;
	MULT_TurnVisible(visibility);
}

void AHunterEnemy::UpdateMovementSpeed(EHunterStates NewHunterState)
{
	switch (NewHunterState)
	{
	case EHunterStates::Attacking:
		GetCharacterMovement()->MaxWalkSpeed = AttackMaxWalkSpeed;
		break;
	case EHunterStates::Running:
		GetCharacterMovement()->MaxWalkSpeed = RunningMaxWalkSpeed;
		break;
	case EHunterStates::Stalking:
		GetCharacterMovement()->MaxWalkSpeed = StalkingMaxWalkSpeed;
		break;
	}
}

void AHunterEnemy::OnDeath(AActor* ActorThatKilled)
{
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController()))
	{
		HunterController->OnHunterDeath(nullptr);
	}
	Super::OnDeath(ActorThatKilled);
	UnPossessed();
	Destroy();
}

void AHunterEnemy::SetChargedAttackOffCooldown()
{
	bChargeAttackCooldown = false;
	
	UWorld* World = GetWorld();
	if (World) World->GetTimerManager().ClearTimer(ChargedAttackCooldownTimerHandle);
}

void AHunterEnemy::MULT_TurnVisible_Implementation(bool visibility)
{
	TrySetVisibiltiyHelper(visibility);
}

bool AHunterEnemy::GetIsChargedAttackOnCooldown()
{
	return bChargeAttackCooldown;
}

bool AHunterEnemy::GetIsAttacking()
{
	return bAttacking;
}

