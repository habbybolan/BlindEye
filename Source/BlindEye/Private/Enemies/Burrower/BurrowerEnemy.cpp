// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemy.h"

#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/SnapperEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "Enemies/SnapperEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/HealthInterface.h"
#include "Particles/ParticleSystemComponent.h"

ABurrowerEnemy::ABurrowerEnemy()
{
	bReplicates = true;
	SurfacingTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpawnTimeline"));
	SurfacingTimelineComponent->SetIsReplicated(true);
	HideTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("HideTimeline"));
	HideTimelineComponent->SetIsReplicated(true);
}

void ABurrowerEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Timeline curve for appearing from the ground
	if (SurfacingCurve)
	{
		SurfacingUpdateEvent.BindUFunction(this, FName("TimelineSurfacingMovement"));
		SurfacingFinishedEvent.BindUFunction(this, FName("TimelineSurfacingFinished"));
		SurfacingTimelineComponent->SetTimelineFinishedFunc(SurfacingFinishedEvent);
		SurfacingTimelineComponent->AddInterpFloat(SurfacingCurve, SurfacingUpdateEvent);
	}

	// Timeline curve for hiding the burrower
	if (HideCurve)
	{
		HideUpdateEvent.BindUFunction(this, FName("TimelineHideMovement"));
		HideFinishedEvent.BindUFunction(this, FName("TimelineHideFinished"));
		HideTimelineComponent->SetTimelineFinishedFunc(HideFinishedEvent);
		HideTimelineComponent->AddInterpFloat(HideCurve, HideUpdateEvent);
	}
	//
	// GetCapsuleComponent()->SetEnableGravity(false);
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABurrowerEnemy::StartSurfacing()
{
	CachedSpawnLocation = GetActorLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
	SetActorLocation(CachedSpawnLocation);
	SetSurfacingHiding();
	SurfacingTimelineComponent->PlayFromStart();
}

void ABurrowerEnemy::StartHiding()
{
	CachedBeforeHidePosition = GetActorLocation();
	HideTimelineComponent->PlayFromStart();
}

void ABurrowerEnemy::SpawnSnappers()
{
	UWorld* world = GetWorld();
	if (!world) return;

	TArray<FVector> spawnPoints = GetSnapperSpawnPoints();
	if (spawnPoints.Num() == 0) return;

	FActorSpawnParameters params;
	params.Owner = this;
	params.Instigator = this;

	// TODO: Make sure number of enemies to spawn doesn't exceed spawn points calculated
	for (int i = 0; i < MinSnappersSpawn; i++)
	{
		uint32 randSpawnIndex = FMath::RandRange(0, spawnPoints.Num() - 1);
		ASnapperEnemy* SpawnedSnapper = world->SpawnActor<ASnapperEnemy>(SnapperType, spawnPoints[randSpawnIndex], GetActorRotation(), params);
		SpawnedSnappers.Add(SpawnedSnapper->GetUniqueID(), SpawnedSnapper);
		spawnPoints.RemoveAt(randSpawnIndex);
		// TODO: subscribe to death event on snapper to remove from list
		if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedSnapper))
		{
			HealthInterface->Execute_GetHealthComponent(SpawnedSnapper)->OnDeathDelegate.AddUFunction<ABurrowerEnemy>(this, FName("OnSnapperDeath"));
		}
	}

	//world->GetTimerManager().SetTimer(HideTimerHandle, this, &ABurrowerEnemy::StartHideLogic, SpawnTimeAppearingLength, false);
}

void ABurrowerEnemy::Destroyed()
{
	Super::Destroyed();
	MULT_DespawnWarningParticle();
}

// void ABurrowerEnemy::SpawnAction(FTransform SpawnLocation)
// {
// 	// TODO: Find spawnpoint, teleport to below it so not showing, rise uit so not showing, p from ground and spawn enemies
// 	CachedSpawnLocation = SpawnLocation.GetLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
// 	SetActorLocation(CachedSpawnLocation);
// 	MULT_SetSurfacingHiding();
// 	SurfacingTimelineComponent->PlayFromStart();
// 	MULT_SpawnWarningParticle();
// }

// void ABurrowerEnemy::AttackAction(ABlindEyePlayerCharacter* target)
// {
// 	UWorld* world = GetWorld();
// 	if (!world) return;
// 	
// 	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(GetController());
// 	if (!BurrowerController) return;
//
// 	UNavigationSystemV1* NavSyst = UNavigationSystemV1::GetNavigationSystem(world);
// 	FNavLocation NavLocation;
// 	NavSyst->GetRandomPointInNavigableRadius(target->GetActorLocation(), 1000, NavLocation);
// 	SetActorLocation(NavLocation.Location + (FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
// 	
// 	MULT_SetFollowing();
// 	BurrowerController->MoveToActor(target, 1);
// 	world->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABurrowerEnemy::StartAttackAppearance, MaxTimerFollowingPlayerInAttack, false);
// 	MULT_SpawnFollowParticle();
// }

void ABurrowerEnemy::OnSnapperDeath(AActor* SnapperActor)
{
	SpawnedSnappers.Remove(SnapperActor->GetUniqueID());
}

TArray<FVector> ABurrowerEnemy::GetSnapperSpawnPoints()
{
	TArray<FVector> SpawnPoints;
	
	UWorld* world = GetWorld();
	if (!world) return SpawnPoints;
	
	FHitResult Hit;
	UKismetSystemLibrary::LineTraceSingleForObjects(world, GetActorLocation() + FVector::UpVector * 200,  GetActorLocation() + FVector::ForwardVector * 400 + FVector::DownVector * 1000,
		ObjectTypesTraceSpawner, false, TArray<AActor*>(), EDrawDebugTrace::None, Hit, true);

	FVector HitLocation = Hit.Location;
	// Get surrounding points from raycast hit as possible snapper spawn points
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			SpawnPoints.Add(FVector(HitLocation.X + 200 * i, HitLocation.Y + 200 * j, HitLocation.Z + 100));
		}
	}
	return SpawnPoints;
}

// void ABurrowerEnemy::StartAttackAppearance()
// {
// 	UWorld* world = GetWorld();
// 	if (!world) return;
//
// 	// clear timer for ending attack follow
// 	world->GetTimerManager().ClearTimer(AttackTimerHandle);
// 	world->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABurrowerEnemy::PerformAttackAppearance, AttackDelayBeforeEmerging, false);
// }

// void ABurrowerEnemy::PerformAttackAppearance()
// {
// 	SpawnAction(GetActorTransform());
// }

void ABurrowerEnemy::TimelineSurfacingMovement(float Value)
{
	SetActorLocation(FMath::Lerp(CachedSpawnLocation, CachedSpawnLocation +
		(FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50)), Value));
}

void ABurrowerEnemy::TimelineSurfacingFinished()
{
	// //GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// // TODO: play particles and delay resurface
	// ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(GetController());
	// if (!BurrowerController) return;
	//
	// MULT_DespawnFollowParticle();
	// MULT_DespawnWarningParticle();
	//
	// if (BurrowerController->GetCurrAction() == EBurrowActionState::Attacking)
	// {
	// 	UWorld* world = GetWorld();
	// 	if (!world) return;
	//
	// 	MULT_SetAppeared();
	// 	world->GetTimerManager().SetTimer(HideTimerHandle, this, &ABurrowerEnemy::StartHideLogic, AttackTimeAppearingLength, false);
	// } else
	// {
	// 	SpawnSnappers();
	// 	MULT_SetAppeared();
	// }
	
	SetAppeared();
	SurfacingFinished.ExecuteIfBound();
}

void ABurrowerEnemy::TimelineHideMovement(float Value)
{
	SetActorLocation(FMath::Lerp(CachedBeforeHidePosition, CachedBeforeHidePosition +
		(FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50)), Value));
}

void ABurrowerEnemy::TimelineHideFinished()
{
	SetDisappeared();
	HidingFinished.ExecuteIfBound();
	// ActionStateFinished.ExecuteIfBound();
}

void ABurrowerEnemy::StartHideLogic()
{
	CachedBeforeHidePosition = GetActorLocation();
	HideTimelineComponent->PlayFromStart();
	SetSurfacingHiding();
}
 
void ABurrowerEnemy::MULT_SetBurrowerState_Implementation(bool isHidden, bool bFollowing)
{ 
	SetActorHiddenInGame(isHidden);
	
	GetCapsuleComponent()->SetEnableGravity(bFollowing);
	GetCharacterMovement()->GravityScale = bFollowing ? 1 : 0;
	if (bFollowing)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	} else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	}
}

void ABurrowerEnemy::SetAppeared()
{
	MULT_SetBurrowerState(false, false);
}

void ABurrowerEnemy::SetDisappeared()
{
	MULT_SetBurrowerState(true, false);
}
 
void ABurrowerEnemy::SetSurfacingHiding()
{
	MULT_SetBurrowerState(false, false);
}

void ABurrowerEnemy::MULT_SpawnWarningParticle_Implementation()
{
	UWorld* world = GetWorld();
	SpawnedWarningParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, WarningParticle,
		GetActorLocation() + FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2),
		FRotator::ZeroRotator, FVector::OneVector, true);
}

void ABurrowerEnemy::MULT_DespawnWarningParticle_Implementation()
{
	if (SpawnedWarningParticle != nullptr)
	{
		SpawnedWarningParticle->Deactivate();
	}
}

void ABurrowerEnemy::MULT_SpawnFollowParticle_Implementation()
{
	SpawnedWarningParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(FollowParticle, GetRootComponent(), TEXT("FollowParticle"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
}

void ABurrowerEnemy::MULT_DespawnFollowParticle_Implementation()
{
	if (SpawnedFollowParticle)
	{
		SpawnedFollowParticle->Deactivate();
	}
}
