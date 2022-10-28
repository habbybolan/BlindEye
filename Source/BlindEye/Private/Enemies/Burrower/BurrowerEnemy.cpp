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

	SetDisappeared();

	CachedCollisionObject = GetCapsuleComponent()->GetCollisionObjectType();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	CachedMeshRelativeLocation = GetMesh()->GetRelativeLocation();

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

void ABurrowerEnemy::OnDeath(AActor* ActorThatKilled)
{
	Super::OnDeath(ActorThatKilled); 
}

void ABurrowerEnemy::SpawnMangerSetup(uint8 islandID, TScriptInterface<IBurrowerSpawnManagerListener> listener)
{ 
	IslandID = islandID;
	Listener = listener;
}

void ABurrowerEnemy::StartSurfacing()
{
	GetCapsuleComponent()->SetCollisionObjectType(CachedCollisionObject);
	MULT_StartSurfacingHelper();
	PerformSurfacingDamage();
	MULT_PlaySurfacingAnimation();
}

void ABurrowerEnemy::MULT_StartSurfacingHelper_Implementation()
{
	SetSurfacingHiding();
	SurfacingTimelineComponent->PlayFromStart();
}

void ABurrowerEnemy::PerformSurfacingDamage()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	FVector StartPosition = GetMesh()->GetComponentLocation();
	FVector EndPosition = GetMesh()->GetComponentLocation() + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(World, StartPosition, EndPosition, SurfacingRadius, SurfacingObjectTypes,
		false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);

	for (FHitResult Hit : OutHits)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), SurfacingDamage, Hit.Location, Hit, GetController(), this, SurfacingDamageType);
	}
}

void ABurrowerEnemy::StartHiding()
{
	// If currently surfacing, start hiding from current position
	if (bIsSurfacing || bIsHiding)
	{
		// Calculate the percentage the burrower has lifted from target hide position
		float timelineLength = HideTimelineComponent->GetTimelineLength();
		float DistToHidingTarget = FVector::Distance(GetMesh()->GetRelativeLocation(), GetHidePosition());
		float FullDistFromSurfacedToHiding = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
		float StartTime = (1 - (DistToHidingTarget / FullDistFromSurfacedToHiding)) * timelineLength;

		MULT_StartHidingHelper(StartTime);
	}
	// Fully surfaced, hid normally
	else
	{
		MULT_StartHidingHelper(0);
	}
	
	MULT_PlaySurfacingAnimation();
}

void ABurrowerEnemy::MULT_StartHidingHelper_Implementation(float StartTime)
{
	// If currently surfacing, start hiding from current position
	if (StartTime > 0)
	{
		// stop surfacing if it was surfacing
		SurfacingTimelineComponent->Stop();
		bIsSurfacing = false;

		// Play from current position to target hide position
		HideTimelineComponent->SetPlaybackPosition(StartTime, true);
		HideTimelineComponent->Play();
	}
	// Fully surfaced, hid normally
	else
	{
		HideTimelineComponent->PlayFromStart();
	}
}

void ABurrowerEnemy::SpawnSnappers()
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	FVector location = GetMesh()->GetBoneLocation(TEXT("Mouth"));
	FRotator rotation = GetMesh()->GetBoneQuaternion(TEXT("Mouth")).Rotator();
	
	World->SpawnActor<ASnapperEnemy>(SnapperType, location, rotation);
	
	// TArray<FVector> spawnPoints = GetSnapperSpawnPoints();
	// if (spawnPoints.Num() == 0) return;
	//
	// FActorSpawnParameters params;
	// params.Owner = this;
	// params.Instigator = this;
	// params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	// // TODO: Make sure number of enemies to spawn doesn't exceed spawn points calculated
	// for (int i = 0; i < MinSnappersSpawn; i++)
	// {
	// 	uint32 randSpawnIndex = FMath::RandRange(0, spawnPoints.Num() - 1);
	// 	ASnapperEnemy* SpawnedSnapper = world->SpawnActor<ASnapperEnemy>(SnapperType, spawnPoints[randSpawnIndex], GetActorRotation(), params);
	// 	if (SpawnedSnapper == nullptr) continue;
	// 	SpawnedSnappers.Add(SpawnedSnapper->GetUniqueID(), SpawnedSnapper);
	// 	spawnPoints.RemoveAt(randSpawnIndex);
	//
	// 	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedSnapper))
	// 	{
	// 		HealthInterface->GetHealthComponent()->OnDeathDelegate.AddUFunction<ABurrowerEnemy>(this, FName("OnSnapperDeath"));
	// 	} 
	// }
}

FVector ABurrowerEnemy::GetHidePosition()
{
	return CachedMeshRelativeLocation + FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2);
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

void ABurrowerEnemy::MULT_PlaySurfacingAnimation_Implementation()
{
	PlayAnimMontage(SurfacingAnimation);
}

void ABurrowerEnemy::TimelineSurfacingMovement(float Value)
{
	bIsSurfacing = true;
	FVector StartLocation = GetHidePosition();
	GetMesh()->SetRelativeLocation(FMath::Lerp(StartLocation, StartLocation +
		(FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2)), Value));
}

void ABurrowerEnemy::TimelineSurfacingFinished()
{
	bIsSurfaced = true;
	bIsSurfacing = false;
	SetAppeared();
	SurfacingFinished.ExecuteIfBound();
}

void ABurrowerEnemy::TimelineHideMovement(float Value)
{
	bIsHiding = true;
	GetMesh()->SetRelativeLocation(FMath::Lerp(CachedMeshRelativeLocation, CachedMeshRelativeLocation +
		(FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2)), Value));
}

void ABurrowerEnemy::TimelineHideFinished()
{
	bIsSurfaced = false;
	bIsHiding = false;
	SetDisappeared();
	HidingFinished.ExecuteIfBound();
	HealthComponent->RemoveMark();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
}
 
void ABurrowerEnemy::MULT_SetBurrowerState_Implementation(bool isHidden, bool bFollowing)
{
	GetMesh()->SetHiddenInGame(isHidden);
	
	if (bFollowing)
	{
		MULT_SpawnFollowParticle();
	} else
	{
		MULT_DespawnFollowParticle();
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
		GetCapsuleComponent()->GetComponentLocation() + FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
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
	SpawnedWarningParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(FollowParticle, GetCapsuleComponent(), TEXT("FollowParticle"),
		FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
}

void ABurrowerEnemy::MULT_DespawnFollowParticle_Implementation()
{
	if (SpawnedFollowParticle)
	{
		SpawnedFollowParticle->Deactivate();
	}
}

void ABurrowerEnemy::CancelHide()
{
	if (bIsHiding)
	{
		MULT_CancelHideHelper();
	}
}

void ABurrowerEnemy::MULT_CancelHideHelper_Implementation()
{
	HideTimelineComponent->Stop();
}

bool ABurrowerEnemy::GetIsSurfaced()
{
	return bIsSurfaced;
}

bool ABurrowerEnemy::GetIsSurfacing()
{
	return bIsSurfacing;
}

bool ABurrowerEnemy::GetIsHiding()
{
	return bIsHiding;
}

bool ABurrowerEnemy::GetIsHidden()
{
	return !bIsSurfaced && !bIsSurfacing && !bIsHiding;
}

float ABurrowerEnemy::PlaySpawnSnapperAnimation()
{
	MULT_PlaySpawnSnapperAnimationHelper();
	return SpawnSnapperAnimation->GetPlayLength();
}

void ABurrowerEnemy::MULT_PlaySpawnSnapperAnimationHelper_Implementation()
{
	PlayAnimMontage(SpawnSnapperAnimation);
}
