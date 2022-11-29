// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperEnemy.h"

#include "BrainComponent.h"
#include "Shrine.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/Snapper/SnapperEnemyController.h"
#include "Enemies/Snapper/SnapperHealthComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ASnapperEnemy::ASnapperEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USnapperHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	RagdollCapsule = CreateDefaultSubobject<UCapsuleComponent>("Ragdoll Capsule");
	RagdollCapsule->SetupAttachment(GetCapsuleComponent());
	RagdollCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASnapperEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bRagdolling && !bGettingUp)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			TeleportColliderToMesh(DeltaSeconds);
		} else
		{
			//UpdateHipLocation(DeltaSeconds);
		}
	}

	RotateWhileJumping(DeltaSeconds);
}

void ASnapperEnemy::RotateWhileJumping(float DeltaSeconds)
{
	if (GetCharacterMovement()->IsFalling())
	{
		FRotator TargetRotation = GetCharacterMovement()->Velocity.Rotation();
		FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, JumpingBetweenIslandsRInterpSpeed);
		NewRotation.Roll = 0;
		NewRotation.Pitch = 0;
		SetActorRotation(NewRotation);
	}
}

void ASnapperEnemy::MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType,
                                   AActor* DamageCauser)
{
	Super::MYOnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);

	if (GetLocalRole() < ROLE_Authority) return;
	
	if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
	{
		SnapperController->DamageTaken(Damage, HitLocation, DamageType, DamageCauser);
		if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(DamageCauser))
		{
			if (SubscribedShrineAttackPoint)
			{
				SubscribedShrineAttackPoint->UnsubscribeSnapper();
				SubscribedShrineAttackPoint = nullptr;
			}
			MULT_OnPlayerAttackedSnapper();
		}
	}
}

void ASnapperEnemy::MULT_OnPlayerAttackedSnapper_Implementation()
{
	// If snapper basic attacking shrine, stop animation
	if (GetMesh()->GetAnimInstance()->Montage_IsActive(BasicAttackAnim))
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(.5f);
	}
}

void ASnapperEnemy::BeginPlay()
{
	Super::BeginPlay();

	CachedCollisionObject = GetCapsuleComponent()->GetCollisionObjectType();
	
	GetCharacterMovement()->GravityScale *= GravityScaleAlteredOnSpawn;

	if (GetLocalRole() == ROLE_Authority)
	{
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASnapperEnemy::CollisionWithGround);
	}

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ASnapperEnemy::OnAnimMontageEnded);

	ApplyKnockBack(QueuedSpawnForce);
}

void ASnapperEnemy::CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// On collision with the ground
	if (bIsSpawning && Hit.bBlockingHit)
	{
		// Unsubscribe to spawning collision delegate
		MULT_OnSpawnCollisionHelper();
	}
	// Hit ground after jumping attack
	else if (CurrAttack == ESnapperAttacks::JumpAttack)
	{
		StopJumpAttack();
	}
} 

void ASnapperEnemy::MULT_OnSpawnCollisionHelper_Implementation()
{
	// set ragdolling
	//TryRagdoll(true, true);
	bIsSpawning = false;
	
	// Update values back to normal
	GetCharacterMovement()->GravityScale /= GravityScaleAlteredOnSpawn;
}

void ASnapperEnemy::PerformJumpAttack()
{
	// prevent attacking while ragdolling or in the air
	if (bRagdolling) return;
	if (GetMovementComponent()->IsFalling()) return;

	CurrAttack = ESnapperAttacks::JumpAttack;
	IsJumpAttackOnDelay = true;
	MULT_PerformJumpAttackHelper();
}

void ASnapperEnemy::SetCanJumpAttack()
{
	IsJumpAttackOnDelay = false;
}

void ASnapperEnemy::SetCanBasicAttack()
{
	IsBasicAttackOnDelay = false;
}

void ASnapperEnemy::PerformBasicAttack()
{
	if (bRagdolling) return;
	if (!IsBasicAttackOnDelay)
	{
		CurrAttack = ESnapperAttacks::BasicAttack;
		IsBasicAttackOnDelay = true;
		MULT_PerformBasicAttackHelper();
	}
}

void ASnapperEnemy::MULT_PerformBasicAttackHelper_Implementation()
{
	PlayAnimMontage(BasicAttackAnim);
}

void ASnapperEnemy::TempLaunch()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * 50000 + FVector::UpVector * 50000);
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void ASnapperEnemy::LaunchSwing()
{
	// UWorld* world = GetWorld();
	// if (!world) return;
	//
	// ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController());
	// if (BlindEyeController == nullptr) return;
	//
	// AActor* Target = BlindEyeController->GetBTTarget();
	// if (Target == nullptr) return;
	//
	// FVector Direction = Target->GetActorLocation() - GetActorLocation();
	// Direction.Normalize();
	//
	// TArray<FHitResult> OutHits;
	// UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorLocation() + Direction * 300, FVector(0, 100 / 2, 100 / 2),
	// 	GetActorRotation(), PlayerObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	//
	// for (FHitResult Hit : OutHits)
	// {
	// 	AActor* HitActor = Hit.GetActor();
	// 	if (!HitActor) continue;
	//
	// 	UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, JumpAttackDamageType);
	// }
}

bool ASnapperEnemy::GetIsRagdolling()
{
	return bRagdolling;
}

bool ASnapperEnemy::GetIsSpawning()
{
	return bIsSpawning;
}

void ASnapperEnemy::ApplyKnockBack(FVector Force)
{
	if (bRagdolling)
	{
		GetMesh()->AddImpulseAtLocation(Force, GetMesh()->GetBoneLocation("Hips"), "Hips");
		UWorld* World = GetWorld();
		if (World)
		{
			// reset ragdoll timer if force applied
			GetWorldTimerManager().SetTimer(StopRagdollTimerHandle, this, &ASnapperEnemy::MULT_StopRagdoll, RagdollDuration, false);
		}
	} else
	{
		TryRagdoll(true, bIsSpawning);
		GetMesh()->AddImpulseAtLocation(Force, GetMesh()->GetBoneLocation("Hips"), "Hips");
	}
} 

void ASnapperEnemy::TryRagdoll(bool SimulatePhysics, bool IsIndefiniteRagdoll)
{
	// Prevent calling ragdoll again, reset timer to get up
	if (bRagdolling == SimulatePhysics)
	{
		return;
	}

	if (SimulatePhysics)
	{ 
		GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
		MULT_StartRagdoll(IsIndefiniteRagdoll);
	} else
	{
		MULT_StopRagdoll();
	}
}

void ASnapperEnemy::BeginStopRagdollTimer()
{
	GetWorldTimerManager().SetTimer(StopRagdollTimerHandle, this, &ASnapperEnemy::MULT_StopRagdoll, RagdollDuration, false);
}

void ASnapperEnemy::ManualStopRagdollTimer(float Duration)
{
	GetWorldTimerManager().SetTimer(StopRagdollTimerHandle, this, &ASnapperEnemy::MULT_StopRagdoll, Duration, false);
}

void ASnapperEnemy::TeleportColliderToMesh(float DeltaSeconds)
{
	FVector TargetLocation = GetMesh()->GetSocketLocation(TEXT("Hips")) + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector TeleportLocation = UKismetMathLibrary::VInterpTo(GetCapsuleComponent()->GetComponentLocation(), TargetLocation, DeltaSeconds, 5);
	GetCapsuleComponent()->SetWorldLocation(TeleportLocation);
	FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("Hips"));
	GetCapsuleComponent()->SetWorldRotation(FRotator(0, SocketRotation.Yaw, 0));
	HipLocation = TargetLocation;
}

void ASnapperEnemy::UpdateHipLocation(float DeltaSecond)
{
	FVector HipForceDirection = HipLocation - GetMesh()->GetSocketLocation(TEXT("Hips"));
	GetMesh()->AddForceToAllBodiesBelow(HipForceDirection * 150, "Hips");
	
	// TODO: Apply rotational force to align with capsule?
	// TODO: Apply more force while in the air, so relatively similar once hit the ground?

	float Dist = UKismetMathLibrary::Vector_Distance(GetMesh()->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation());
	if (Dist < 50) return;
	FVector LerpedLocation = UKismetMathLibrary::VLerp(GetMesh()->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation(), DeltaSecond);
	GetMesh()->SetWorldLocation(LerpedLocation, false, nullptr, ETeleportType::TeleportPhysics);
} 

void ASnapperEnemy::MULT_StartRagdoll_Implementation(bool IsIndefiniteRagdoll)
{
	GetMovementComponent()->SetComponentTickEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (GetLocalRole() == ROLE_Authority) 
	{  
		bRagdolling = true;
		// if not indefinite, set timer to stop ragdolling
		if(!IsIndefiniteRagdoll)
		{
			BeginStopRagdollTimer();
		}
	} 
	GetMesh()->SetPhysicsBlendWeight(1);
}

void ASnapperEnemy::MULT_StopRagdoll_Implementation() 
{
	// Prevent getting up if dead
	if (bIsDead) return;
	
	bGettingUp = true;
	GetMovementComponent()->StopMovementImmediately();
	GetMovementComponent()->SetComponentTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// Play getup montage
	float TimeForGetup;
	if (IsLayingOnFront())
	{
		TimeForGetup = PlayAnimMontage(GetUpFromInFrontMontage);
	} else
	{
		TimeForGetup = PlayAnimMontage(GetUpFromBehindMontage);
	}

	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	GetWorldTimerManager().SetTimer(GetupAnimTimerHandle, this, &ASnapperEnemy::FinishGettingUp, TimeForGetup, false);
	
	AlphaBlendWeight = 1;
	// blend weight loop to smooth out getting up animation
	GetWorldTimerManager().SetTimer(PhysicsBlendWeightTimerHandle, this, &ASnapperEnemy::SetPhysicsBlendWeight, BlendWeightDelay, true);
}

void ASnapperEnemy::FinishGettingUp()
{
	PlayAnimMontage(GetupRoarAnim);
}

void ASnapperEnemy::OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	// Getup Roar animation
	if (Montage == GetupRoarAnim)
	{
		bGettingUp = false;
		bRagdolling = false;
	}

	if (Montage == JumpAttackAnim)
	{
		if (CurrAttack == ESnapperAttacks::JumpAttack)
		{
			StopJumpAttack();
		}
	}

	if (Montage == BasicAttackAnim)
	{
		if (CurrAttack == ESnapperAttacks::BasicAttack)
		{
			StopBasicAttack();
		}
	}
}

void ASnapperEnemy::StopJumpAttack()
{
	CurrAttack = ESnapperAttacks::None;
	GetMesh()->GetAnimInstance()->Montage_Stop(0, JumpAttackAnim);
	//LaunchSwing();
	IsJumpAttackOnDelay = true;
	GetWorldTimerManager().SetTimer(JumpAttackDelayTimerHandle, this, &ASnapperEnemy::SetCanJumpAttack, JumpAttackDelay, false);
	MULT_StopJumpAttackHelper();
}

void ASnapperEnemy::MULT_StopJumpAttackHelper_Implementation()
{
	TryRagdoll(true);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASnapperEnemy::StopBasicAttack()
{
	CurrAttack = ESnapperAttacks::None;
	GetWorldTimerManager().SetTimer(BasicAttackDelayTimerHandle, this, &ASnapperEnemy::SetCanBasicAttack, BasicAttackDelay, false);
}

void ASnapperEnemy::UnsubFromShrineAttackPoint()
{
	SubscribedShrineAttackPoint = nullptr;
}

void ASnapperEnemy::SubToShrineAttackPoint(UShrineAttackPoint* ShrineAttackPoint)
{
	SubscribedShrineAttackPoint = ShrineAttackPoint;
}

UShrineAttackPoint* ASnapperEnemy::GetShrineAttackPoint()
{
	return SubscribedShrineAttackPoint;
}

void ASnapperEnemy::SetPhysicsBlendWeight()
{
	if (AlphaBlendWeight <= 0)
	{
		GetMesh()->SetSimulatePhysics(false);
		GetWorldTimerManager().ClearTimer(PhysicsBlendWeightTimerHandle);
		return;
	}

	AlphaBlendWeight = UKismetMathLibrary::FInterpTo(AlphaBlendWeight, 0, BlendWeightDelay, 5);
	GetMesh()->SetPhysicsBlendWeight(AlphaBlendWeight);
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
		EDrawDebugTrace::None, OutHit, true);
}

void ASnapperEnemy::MULT_PerformJumpAttackHelper_Implementation()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0);
	PlayAnimMontage(JumpAttackAnim);
	TempLaunch();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASnapperEnemy::OnDeath(AActor* ActorThatKilled)
{
	if (bIsDead) return;
	
	Super::OnDeath(ActorThatKilled);

	TryRagdoll(true);

	if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(Controller))
	{
		SnapperController->OnSnapperDeath();
	}
}

void ASnapperEnemy::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( ASnapperEnemy, bRagdolling );
	DOREPLIFETIME( ASnapperEnemy, HipLocation );
	DOREPLIFETIME( ASnapperEnemy, bGettingUp );
	DOREPLIFETIME( ASnapperEnemy, bIsSpawning );
	DOREPLIFETIME_CONDITION( ASnapperEnemy, QueuedSpawnForce, COND_InitialOnly );
}
