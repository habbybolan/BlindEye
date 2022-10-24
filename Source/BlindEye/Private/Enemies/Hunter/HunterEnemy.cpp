// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Enemies/Hunter/HunterHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AHunterEnemy::AHunterEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHunterHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
}

void AHunterEnemy::PerformJumpAttack()
{
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

void AHunterEnemy::MULT_TurnVisible_Implementation(bool visibility)
{
	TrySetVisibiltiyHelper(visibility);
}

