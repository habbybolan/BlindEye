// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperEnemy.h"

#include "BrainComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/Snapper/SnapperEnemyController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ASnapperEnemy::ASnapperEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void ASnapperEnemy::MYOnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
                                                AActor* DamageCauser)
{
	Super::MYOnTakeDamage_Implementation(Damage, HitLocation, DamageType, DamageCauser);

	if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
	{
		SnapperController->DamageTaken(Damage, HitLocation, DamageType, DamageCauser);
	}
}

void ASnapperEnemy::PerformJumpAttack()
{
	if (bRagdolling) return;
	TempLaunch();
	GetWorldTimerManager().SetTimer(LaunchSwingTimerHandle, this, &ASnapperEnemy::LaunchSwing, 0.2, false);
}

void ASnapperEnemy::PerformBasicAttack()
{
	if (bRagdolling) return;

	UWorld* world = GetWorld();
	if (!world) return;

	ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController());
	if (BlindEyeController == nullptr) return;

	AActor* Target = BlindEyeController->GetBTTarget();
	if (Target == nullptr) return;

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorForwardVector() * 300, FVector(0, 100 / 2, 100 / 2),
		GetActorRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);
	
	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
	
		UGameplayStatics::ApplyPointDamage(HitActor, BasicAttackDamage
			, Hit.ImpactNormal, Hit, GetController(), this, BasicAttackDamageType);
	}
}

void ASnapperEnemy::TempLaunch()
{
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			FVector Direction = Target->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			Direction += FVector::UpVector * .4f;
			GetCharacterMovement()->AddImpulse(Direction * 50000);
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
	
		UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, JumpAttackDamageType);
	}

	TryRagdoll(true);
}

void ASnapperEnemy::TryRagdoll(bool SimulatePhysics)
{
	// prevent setting ragdoll state if already set
	if (bRagdolling == SimulatePhysics) return;

	if (SimulatePhysics)
	{
		MULT_StartRagdoll();
	} else
	{
		MULT_StopRagdoll();
	}
}

void ASnapperEnemy::TeleportColliderToMesh()
{
	FVector TeleportLocation = GetMesh()->GetSocketLocation(TEXT("Hips"));
	GetCapsuleComponent()->SetWorldLocation(TeleportLocation);
	HipLocation = TeleportLocation;
}

void ASnapperEnemy::UpdateHipLocation()
{
	FVector HipForceDirection = HipLocation - GetMesh()->GetSocketLocation(TEXT("Hips"));
	GetMesh()->AddForceToAllBodiesBelow(HipForceDirection * 150, "Hips");

	float Dist = UKismetMathLibrary::Vector_Distance(GetMesh()->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation());
	if (Dist < 50) return;
	FVector LerpedLocation = UKismetMathLibrary::VLerp(GetMesh()->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation(), 0.05);
	GetMesh()->SetWorldLocation(LerpedLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void ASnapperEnemy::MULT_StartRagdoll_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bRagdolling = true;
		AAIController* AIController = Cast<AAIController>(GetController());
		AIController->GetBrainComponent()->PauseLogic(TEXT("AnimationMontage"));
		GetWorldTimerManager().SetTimer(ColliderOnMeshTimerHandle, this, &ASnapperEnemy::TeleportColliderToMesh, 0.05, true);
		GetWorldTimerManager().SetTimer(StopRagdollTimerHandle, this, &ASnapperEnemy::MULT_StopRagdoll, 5, false);
	} else
	{
		GetWorldTimerManager().SetTimer(ColliderOnMeshTimerHandle, this, &ASnapperEnemy::UpdateHipLocation, 0.05, true);
	}
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	
}

void ASnapperEnemy::MULT_StopRagdoll_Implementation() 
{
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

	// return to normal settings and reattach
	GetMesh()->SetSimulatePhysics(false);

	FRotator HipRotation = GetMesh()->GetSocketRotation("Hips");
	GetCapsuleComponent()->SetWorldRotation(FRotator(0, HipRotation.Yaw, 0));
	GetCharacterMovement()->GravityScale = 1;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), Rules);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -50.0), FRotator(0, -90, 0));

	GetWorldTimerManager().ClearTimer(ColliderOnMeshTimerHandle);
	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(GetupAnimTimerHandle, this, &ASnapperEnemy::FinishGettingUp, TimeForGetup, false);
	}
}

void ASnapperEnemy::FinishGettingUp()
{
	bRagdolling = false;
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

void ASnapperEnemy::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( ASnapperEnemy, bRagdolling );
	DOREPLIFETIME( ASnapperEnemy, HipLocation );
}
