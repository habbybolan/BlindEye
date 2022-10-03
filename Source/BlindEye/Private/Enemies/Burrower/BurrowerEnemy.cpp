// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemy.h"

#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Snapper/SnapperEnemyController.h"
#include "Enemies/Burrower/BurrowerHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/HealthInterface.h"
#include "Kismet/GameplayStatics.h"

ABurrowerEnemy::ABurrowerEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBurrowerHealthComponent>(TEXT("HealthComponent")))
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
}

void ABurrowerEnemy::DestroyBurrower()
{
	Destroy();
}

void ABurrowerEnemy::OnDeath(AActor* ActorThatKilled)
{
	Super::OnDeath(ActorThatKilled); 
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	World->GetTimerManager().SetTimer(DeathTimerHandle, this, &ABurrowerEnemy::DestroyBurrower, DeathDelay, false);
}

void ABurrowerEnemy::StartSurfacing()
{
	MULT_StartSurfacingHelper();
	PerformSurfacingDamage();
}

void ABurrowerEnemy::MULT_StartSurfacingHelper_Implementation()
{
	CachedSpawnLocation = GetActorLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
	SetActorLocation(CachedSpawnLocation);
	SetSurfacingHiding();
	SurfacingTimelineComponent->PlayFromStart();
}

void ABurrowerEnemy::PerformSurfacingDamage()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	FVector StartPosition = CachedSpawnLocation;
	FVector EndPosition = CachedSpawnLocation + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(World, StartPosition, EndPosition, SurfacingRadius, SurfacingObjectTypes,
		false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);

	for (FHitResult Hit : OutHits)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), SurfacingDamage, Hit.Location, Hit, GetController(), this, SurfacingDamageType);
	}
}

void ABurrowerEnemy::StartHiding()
{
	MULT_StartHidingHelper();
}

void ABurrowerEnemy::MULT_StartHidingHelper_Implementation()
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
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedSnapper))
		{
			HealthInterface->GetHealthComponent()->OnDeathDelegate.AddUFunction<ABurrowerEnemy>(this, FName("OnSnapperDeath"));
		} 
	}
}

void ABurrowerEnemy::Destroyed()
{
	Super::Destroyed();
	MULT_DespawnWarningParticle();
}

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

void ABurrowerEnemy::TimelineSurfacingMovement(float Value)
{
	SetActorLocation(FMath::Lerp(CachedSpawnLocation, CachedSpawnLocation +
		(FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 - 15)), Value));
}

void ABurrowerEnemy::TimelineSurfacingFinished()
{
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
	SetActorLocation(FVector::ZeroVector);
	HealthComponent->RemoveMark();
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
		GetActorLocation(),FRotator::ZeroRotator, FVector::OneVector, true);
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
