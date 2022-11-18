// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperEnemy.h"

#include "BrainComponent.h"
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

	// if (bIsSpawning && !GetCharacterMovement()->IsFalling())
	// {
	// 	bIsSpawning = false;
	// 	OnSpawnCollisionHelper();
	// }
}

void ASnapperEnemy::MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType,
                                   AActor* DamageCauser)
{
	Super::MYOnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);

	if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
	{
		SnapperController->DamageTaken(Damage, HitLocation, DamageType, DamageCauser);
	}
}

void ASnapperEnemy::BeginPlay()
{
	Super::BeginPlay();

	CachedCollisionObject = GetCapsuleComponent()->GetCollisionObjectType();
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * ColliderHeightAlteredOnSpawn);
	GetCharacterMovement()->GravityScale *= GravityScaleAlteredOnSpawn;

	if (GetLocalRole() == ROLE_Authority)
	{
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASnapperEnemy::SpawnCollisionWithGround);
	}

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ASnapperEnemy::OnAnimMontageEnded);
}

void ASnapperEnemy::SpawnCollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// On collision with the ground
	if (Hit.bBlockingHit)
	{
		// Unsubscribe to spawning collision delegate
		MULT_OnSpawnCollisionHelper();
	}
} 

void ASnapperEnemy::MULT_OnSpawnCollisionHelper_Implementation()
{
	GetCapsuleComponent()->OnComponentHit.Remove(this, TEXT("SpawnCollisionWithGround"));
	
	// set ragdolling
	TryRagdoll(true, true);
	bIsSpawning = false;
	
	// Update values back to normal
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / ColliderHeightAlteredOnSpawn);
	GetCharacterMovement()->GravityScale /= GravityScaleAlteredOnSpawn;
}

void ASnapperEnemy::PerformJumpAttack()
{
	if (bRagdolling) return;
	
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
		GetActorRotation(), PlayerObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	
	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
	
		UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, JumpAttackDamageType);
	}
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
	// if (bRagdolling)
	// {
	// 	GetMesh()->AddImpulse(Force);
	// } else
	// {
	// 	if (Force.Size() < 500)
	// 	{
	// 		GetCharacterMovement()->AddImpulse(Force);
	// 	} else
	// 	{
	// 		TryRagdoll(true);
	// 		GetMesh()->AddImpulse(Force);
	// 	}
	// }

	if (!bRagdolling)
	{
		TryRagdoll(true);
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
		GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = true;
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
	
	FVector TeleportLocation = UKismetMathLibrary::VInterpTo(TargetLocation, GetCapsuleComponent()->GetComponentLocation(), DeltaSeconds, 1);
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
	GetCapsuleComponent()->SetEnableGravity(false);
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
	GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = false;
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
	// Getup Roar animation
	if (Montage == GetupRoarAnim)
	{
		bGettingUp = false;
		bRagdolling = false;
	}

	if (Montage == JumpAttackAnim)
	{
		TempLaunch();
		LaunchSwing();
		IsJumpAttackOnDelay = true;
		TryRagdoll(true);
		GetWorldTimerManager().SetTimer(JumpAttackDelayTimerHandle, this, &ASnapperEnemy::SetCanJumpAttack, JumpAttackDelay, false);

		GetWorldTimerManager().ClearTimer(PerformingJumpAttackTimerHandle);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (Montage == BasicAttackAnim)
	{
		GetWorldTimerManager().SetTimer(BasicAttackDelayTimerHandle, this, &ASnapperEnemy::SetCanBasicAttack, BasicAttackDelay, false);
	}
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
	PlayAnimMontage(JumpAttackAnim);
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(PerformingJumpAttackTimerHandle, this, &ASnapperEnemy::PerformingJumpAttack, JumpAttackTimerDelay, true);
}

void ASnapperEnemy::PerformingJumpAttack()
{
	FVector ForwardEase = GetActorLocation() + GetActorForwardVector() * JumpForwardSpeed * JumpAttackTimerDelay;
	SetActorLocation(ForwardEase /*+ UpEase*/);
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
}
