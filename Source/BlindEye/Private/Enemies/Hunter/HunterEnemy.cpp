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

	CachedRunningSpeed = GetCharacterMovement()->MaxWalkSpeed;

	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	SetCharged();
}

void AHunterEnemy::PerformChargedJump()
{
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			bAttacking = true;
			bChargeAttackCooldown = true;
			// Have target position land before the target
			FVector DirectionVec = Target->GetActorLocation() - GetActorLocation();
			DirectionVec.Normalize();
			DirectionVec *= ChargedJumpLandingDistanceBeforeTarget;
			
			GetWorldTimerManager().SetTimer(ChargedJumpCooldownTimerHandle, this, &AHunterEnemy::SetChargedJumpOffCooldown, ChargedJumpCooldown, false);
			MULT_PerformChargedJumpHelper(GetActorLocation(), Target->GetActorLocation() - DirectionVec);
		}
	}
}

void AHunterEnemy::MULT_PerformChargedJumpHelper_Implementation(FVector StartLoc, FVector EndLoc)
{
	// Set Start and end locations of jump for Easing
	ChargedJumpStartLocation = StartLoc;
	ChargedJumpTargetLocation = EndLoc;
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(PerformingChargedJumpTimerHandle, this, &AHunterEnemy::PerformingJumpAttack, 0.02, true);
}

void AHunterEnemy::PerformingJumpAttack()
{ 
	FVector ForwardEase = UKismetMathLibrary::VEase(ChargedJumpStartLocation * FVector(1, 1, 0),
		ChargedJumpTargetLocation* FVector(1, 1, 0), CurrTimeOfChargedJump / ChargedJumpDuration, EEasingFunc::Linear);

	FVector UpEase;
	FVector HalfDirectionToTarget = (ChargedJumpTargetLocation - ChargedJumpStartLocation) / 2 + FVector::UpVector * 200;
	float HalfChargedAttackDuration = ChargedJumpDuration / 2;
	// If at first half of jump, jump from starting Z to jump Z-Peak
	if (CurrTimeOfChargedJump / ChargedJumpDuration <= 0.5)
	{
		 UpEase = UKismetMathLibrary::VEase(ChargedJumpStartLocation * FVector::UpVector,
			(ChargedJumpTargetLocation + HalfDirectionToTarget) * FVector::UpVector, CurrTimeOfChargedJump / HalfChargedAttackDuration, EEasingFunc::CircularOut);
	}
	// Other latter half of jump, Go from Jump Z-Peak to end point Z
	else
	{
		UpEase = UKismetMathLibrary::VEase((ChargedJumpTargetLocation + HalfDirectionToTarget) * FVector::UpVector,
		   ChargedJumpTargetLocation * FVector::UpVector,
		   (CurrTimeOfChargedJump - HalfChargedAttackDuration) / (ChargedJumpDuration - HalfChargedAttackDuration), EEasingFunc::CircularIn);
	}

	SetActorLocation(ForwardEase + UpEase);
	CurrTimeOfChargedJump += 0.02;

	if (CurrTimeOfChargedJump >= ChargedJumpDuration)
	{
		GetWorldTimerManager().ClearTimer(PerformingChargedJumpTimerHandle);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrTimeOfChargedJump = 0;
		bAttacking = false;

		if (GetLocalRole() == ROLE_Authority)
		{
			ChargedJumpSwingDamage();
		}
	}
}

void AHunterEnemy::SetCharged()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed;

	World->GetTimerManager().ClearTimer(ChargedCooldownTimerHandle);
	if (ChargedDuration != 0)
	{
		World->GetTimerManager().SetTimer(ChargedDurationTimerHandle, this, &AHunterEnemy::SetNotCharged, ChargedDuration, false);
	}
}

void AHunterEnemy::SetNotCharged()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed * MovementSpeedAlteredDuringNotCharged;
	
	bCharged = false;
	World->GetTimerManager().ClearTimer(ChargedDurationTimerHandle);
	World->GetTimerManager().SetTimer(ChargedCooldownTimerHandle, this, &AHunterEnemy::SetCharged, ChargedCooldown, false);
}

void AHunterEnemy::OnMarkDetonated()
{
	Super::OnMarkDetonated();
	if (GetIsCharged())
	{
		SetNotCharged();
	}
}

void AHunterEnemy::ChargedJumpSwingDamage()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 1000, FVector(0, 100 / 2, 100 / 2),
		GetActorRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
 
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController());
	if (!ensure(HunterController)) return;
	
	AActor* Target = HunterController->GetBTTarget();
	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		// Only allow hitting target
		if (HitActor == Target)
		{
			ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(HitActor);
			if (ensure(BlindEyePlayerCharacter))
			{
				UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, JumpAttackDamageType);
				BlindEyePlayerCharacter->GetHealthComponent()->DetonateDelegate.AddDynamic(this, &AHunterEnemy::OnHunterMarkDetonated);
				BlindEyePlayerCharacter->GetHealthComponent()->MarkedRemovedDelegate.AddDynamic(this, &AHunterEnemy::AHunterEnemy::OnHunterMarkRemoved);
			} 
		}
	}
}

void AHunterEnemy::PerformBasicAttack()
{
	bAttacking = true;
	MULT_PerformBasicAttackHelper();
}

void AHunterEnemy::MULT_PerformBasicAttackHelper_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	float Duration = PlayAnimMontage(BasicAttackAnimation, 1);
	World->GetTimerManager().SetTimer(BasicAttackTimerHandle, this, &AHunterEnemy::SetBasicAttackFinished, Duration, false);
}

void AHunterEnemy::SetBasicAttackFinished()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bAttacking = false;
	}
	StopAnimMontage(BasicAttackAnimation);
}
void AHunterEnemy::OnHunterMarkDetonated()
{
	UnsubscribeToTargetMarks();
	
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	check(HunterController);
	AActor* Target = HunterController->GetBTTarget();
	
	HealthComponent->Stun(5, Target);
	// TODO: Stun Hunter and End jump attack completely
}

void AHunterEnemy::OnHunterMarkRemoved()
{
	UnsubscribeToTargetMarks();
}

void AHunterEnemy::UnsubscribeToTargetMarks()
{
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	check(HunterController);
	AActor* Target = HunterController->GetBTTarget();
	if (Target)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Target);
		check(Player);
		Player->GetHealthComponent()->DetonateDelegate.Remove(this, TEXT("OnHunterMarkDetonated"));
		Player->GetHealthComponent()->MarkedRemovedDelegate.Remove(this, TEXT("OnHunterMarkRemoved"));
	}
}

void AHunterEnemy::TrySetVisibility(bool visibility)
{
	if (IsVisible == visibility) return;

	IsVisible = visibility;
	MULT_TurnVisible(visibility);
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

void AHunterEnemy::SetChargedJumpOffCooldown()
{
	bChargeAttackCooldown = false;
	
	UWorld* World = GetWorld();
	if (World) World->GetTimerManager().ClearTimer(ChargedJumpCooldownTimerHandle);
}

void AHunterEnemy::MULT_TurnVisible_Implementation(bool visibility)
{
	TrySetVisibiltiyHelper(visibility);
}

bool AHunterEnemy::GetIsChargedJumpOnCooldown()
{
	return bChargeAttackCooldown;
}

bool AHunterEnemy::GetIsAttacking()
{
	return bAttacking;
}

bool AHunterEnemy::GetIsCharged()
{
	return bCharged;
}

void AHunterEnemy::ApplyBasicAttackDamage(FHitResult Hit, bool IfShouldApplyHunterMark)
{
	// base DamageType and damage amount if Hunter Charged and if marking target
	TSubclassOf<UBaseDamageType> DamageTypeToApply = IfShouldApplyHunterMark ? BasicAttackDamageTypeWithMark : BasicAttackDamageTypeNoMark;
	UGameplayStatics::ApplyPointDamage(Hit.Actor.Get(), BasicAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, DamageTypeToApply);

	// Remove Charged once marked player
	if (bCharged && IfShouldApplyHunterMark)
	{
		SetNotCharged();
	}
}

