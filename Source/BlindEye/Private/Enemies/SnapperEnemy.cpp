// Copyright (C) Nicholas Johnson 2022


#include "Enemies/SnapperEnemy.h"

#include "BrainComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/SnapperEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ASnapperEnemy::ASnapperEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void ASnapperEnemy::MYOnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
                                                AActor* DamageCauser)
{
	Super::MYOnTakeDamage_Implementation(Damage, HitLocation, DamageType, DamageCauser);

	// If taken damage from player, set Target as player
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(DamageCauser))
	{
		if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
		{
			SnapperController->SetTargetEnemy(BlindEyeCharacter);
		}
	}
}

void ASnapperEnemy::PerformBasicAttack()
{
	if (bRagdolling) return;
	TempLaunch();
	GetWorldTimerManager().SetTimer(LaunchSwingTimerHandle, this, &ASnapperEnemy::LaunchSwing, 0.2, false);
}

void ASnapperEnemy::TempLaunch()
{
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			FVector Direction = Target->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			Direction += FVector::UpVector * 1;
			GetCharacterMovement()->AddImpulse(Direction * 30000);
		}
	}
}

void ASnapperEnemy::LaunchSwing()
{
	UWorld* world = GetWorld();
	if (!world) return;

	ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController());
	if (BlindEyeController == nullptr) return;

	AActor* Target = BlindEyeController->GetBTTarget();
	if (Target == nullptr) return;

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorLocation() + Direction * 300, FVector(0, 100 / 2, 100 / 2),
		GetActorRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);
	
	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
	
		UGameplayStatics::ApplyPointDamage(HitActor, BasicAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, BasicAttackDamageType);
	}

	TryRagdoll(true);
}

void ASnapperEnemy::TryRagdoll(bool SimulatePhysics)
{
	// prevent setting ragdoll state if already set
	if (bRagdolling == SimulatePhysics) return;

	if (SimulatePhysics)
	{
		StartRagdoll();
	} else
	{
		StopRagdoll();
	}
}

void ASnapperEnemy::TeleportColliderToMesh()
{
	FVector TeleportLocation = GetMesh()->GetSocketLocation(TEXT("Hips"));
	GetCapsuleComponent()->SetWorldLocation(TeleportLocation);
}

void ASnapperEnemy::StartRagdoll()
{
	bRagdolling = true;
	AAIController* AIController = Cast<AAIController>(GetController());
	AIController->GetBrainComponent()->PauseLogic(TEXT("AnimationMontage"));
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetWorldTimerManager().SetTimer(ColliderOnMeshTimerHandle, this, &ASnapperEnemy::TeleportColliderToMesh, 0.05, true);
	GetWorldTimerManager().SetTimer(StopRagdollTimerHandle, this, &ASnapperEnemy::StopRagdoll, 5, false);
}

void ASnapperEnemy::StopRagdoll()
{
	GetWorldTimerManager().ClearTimer(ColliderOnMeshTimerHandle);

	// return to normal settings and reattach
	bRagdolling = false;
	GetMesh()->SetSimulatePhysics(false);

	FRotator HipRotation = GetMesh()->GetSocketRotation("Hips");
	//GetCapsuleComponent()->SetWorldRotation(FRotator(0, 0, HipRotation.Yaw));
	GetCharacterMovement()->GravityScale = 1;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), Rules);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -50.0), FRotator(0, -90, 0));

	// Play getup montage
	float TimeForGetup;
	if (IsLayingOnFront())
	{
		TimeForGetup = PlayAnimMontage(GetUpFromInFrontMontage);
		TimeForGetup /= 3;
	} else
	{
		TimeForGetup = PlayAnimMontage(GetUpFromBehindMontage);
	}
	GetWorldTimerManager().SetTimer(GetupAnimTimerHandle, this, &ASnapperEnemy::FinishGettingUp, TimeForGetup, false);
}

void ASnapperEnemy::FinishGettingUp()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	AIController->GetBrainComponent()->ResumeLogic(TEXT("AnimationMontage"));
}

bool ASnapperEnemy::IsLayingOnFront()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;

	FVector HipsLocation = GetMesh()->GetSocketLocation(TEXT("Hips"));
	
	FVector HipsFwd = GetMesh()->GetSocketRotation(TEXT("Hips")).Vector();
	FVector HipsDown = UKismetMathLibrary::GetRightVector(GetMesh()->GetSocketRotation(TEXT("Hips")));
	FVector ProperFwd = UKismetMathLibrary::RotateAngleAxis(HipsFwd, -90, HipsDown);
	ProperFwd.Normalize();
 
	FHitResult OutHit;
	return UKismetSystemLibrary::LineTraceSingle(World, HipsLocation, HipsLocation + ProperFwd * 50, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(),
		EDrawDebugTrace::ForDuration, OutHit, true);
}
