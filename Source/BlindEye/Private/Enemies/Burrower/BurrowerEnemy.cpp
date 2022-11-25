// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemy.h"

#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "Enemies/Snapper/SnapperEnemyController.h"
#include "Enemies/Burrower/BurrowerHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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
	
	VisibilityState = EBurrowerVisibilityState::Hidden;
	GetMesh()->SetHiddenInGame(true);

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
	NotifySpawningStopped();
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
	SetVisibility(false);
}

void ABurrowerEnemy::MULT_StartSurfacingHelper_Implementation()
{
	SurfacingTimelineComponent->PlayFromStart();
	BP_SurfacingStarted_CLI();
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
	// If currently surfacing/Hiding, start hiding from current position
	if (VisibilityState == EBurrowerVisibilityState::Surfacing || VisibilityState == EBurrowerVisibilityState::Hiding)
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
}

void ABurrowerEnemy::MULT_StartHidingHelper_Implementation(float StartTime)
{
	GetMesh()->GetAnimInstance()->StopAllMontages(.5);
	// If currently surfacing, start hiding from current position
	if (StartTime > 0)
	{
		// stop surfacing if it was surfacing
		SurfacingTimelineComponent->Stop();
		UpdateBurrowerState(EBurrowerVisibilityState::Hiding);

		// Play from current position to target hide position
		HideTimelineComponent->SetPlaybackPosition(StartTime, true);
		HideTimelineComponent->Play();
	}
	// Fully surfaced, hid normally
	else
	{
		HideTimelineComponent->PlayFromStart();
	}
	BP_HidingStarted_CLI();
}

void ABurrowerEnemy::SpawnSnappers() 
{
	if (GetLocalRole() < ROLE_Authority) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector location = GetMesh()->GetBoneLocation(TEXT("Mouth"));
	FRotator rotation = -1 * GetMesh()->GetBoneQuaternion(TEXT("Mouth")).Rotator();

	rotation.Yaw += SnapperSpawnAngleToLeft;

	FTransform Transform;
	Transform.SetLocation(location);
	Transform.SetRotation(rotation.Quaternion());
	ASnapperEnemy* Snapper = World->SpawnActorDeferred<ASnapperEnemy>(SnapperType, Transform);
	Snapper->QueuedSpawnForce = GetMesh()->GetBoneQuaternion(TEXT("Mouth")).Vector() * ForwardSnapperSpawnForce;
	Snapper->FinishSpawning(Transform);
	if (Snapper)
	{
		SnappersBeingSpawned.Add(Snapper);	
	}
}

FVector ABurrowerEnemy::GetHidePosition()
{
	return CachedMeshRelativeLocation + FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2);
}

void ABurrowerEnemy::Destroyed()
{
	UnsubscribeToSpawnLocation();
	Super::Destroyed();
}

FVector ABurrowerEnemy::GetWorldWarningParticleSpawnLocation()
{
	return GetCapsuleComponent()->GetComponentLocation() + FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

FVector ABurrowerEnemy::GetRelativeFollowParticleSpawnLocation()
{
	return FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

void ABurrowerEnemy::UpdateBurrowerState(EBurrowerVisibilityState NewState)
{
	EBurrowerVisibilityState OldState = VisibilityState;
	VisibilityState = NewState;
	OnRep_VisibilityState(OldState);
}

void ABurrowerEnemy::MULT_WarningStarted_Implementation()
{
	BP_WarningStarted_CLI();
} 

void ABurrowerEnemy::MULT_WarningEnded_Implementation()
{
	BP_WarningEnded_CLI();
}

EBurrowerVisibilityState ABurrowerEnemy::GetVisibilityState()
{
	return VisibilityState;
}

void ABurrowerEnemy::SubscribeToSpawnLocation(UBurrowerSpawnPoint* SpawnPoint)
{
	CurrUsedSpawnPoint = SpawnPoint;
	CurrUsedSpawnPoint->bInUse = true;
}

void ABurrowerEnemy::UnsubscribeToSpawnLocation()
{
	if (CurrUsedSpawnPoint)
	{
		CurrUsedSpawnPoint->bInUse = false;
		CurrUsedSpawnPoint = nullptr;
	}
}

void ABurrowerEnemy::SubscribeToIsland(AIsland* Island)
{
	OwningIsland = Island;
}

UBurrowerSpawnPoint* ABurrowerEnemy::GetRandUnusedSpawnPoint()
{
	// Already at a spawn point, use this one
	if (CurrUsedSpawnPoint != nullptr)
	{
		return CurrUsedSpawnPoint;
	}
	
	if (ensure(OwningIsland))
	{
		UBurrowerSpawnPoint* SpawnPoint = OwningIsland->GetRandUnusedBurrowerSpawnPoint();
		if (ensure(SpawnPoint))
		{
			return SpawnPoint;
		}
	}
	return nullptr;
}

void ABurrowerEnemy::NotifySpawningStopped()
{
	// Set timer for stopping snapper ragdoll with some variability
	for (ASnapperEnemy* Snapper : SnappersBeingSpawned)
	{
		float RandVariability = UKismetMathLibrary::RandomFloatInRange(0, SnapperRagdollTimeVariabilityAfterGroupSpawned);
		Snapper->ManualStopRagdollTimer(SnapperRagdollBaseTimeAfterGroupSpawned + RandVariability);
	}
	SnappersBeingSpawned.Empty();
}

void ABurrowerEnemy::OnRep_VisibilityState(EBurrowerVisibilityState OldVisibilityState) 
{
	GetMesh()->SetHiddenInGame(VisibilityState == EBurrowerVisibilityState::Hidden);
	// If changing from hidden to not hidden
	if (VisibilityState != EBurrowerVisibilityState::Hidden)
	{
		BP_FollowingEnd_CLI();
	}
	// If changing from not hidden to hidden
	else if (VisibilityState == EBurrowerVisibilityState::Hidden)
	{
		BP_FollowingStart_CLI();
	}
}

void ABurrowerEnemy::OnSnapperDeath(AActor* SnapperActor)
{ 
	// TODO: Remove?
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
	UpdateBurrowerState(EBurrowerVisibilityState::Surfacing);
	FVector StartLocation = GetHidePosition();
	GetMesh()->SetRelativeLocation(FMath::Lerp(StartLocation, StartLocation +
		(FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2)), Value));
}

void ABurrowerEnemy::TimelineSurfacingFinished()
{
	UpdateBurrowerState(EBurrowerVisibilityState::Surfaced);
	SurfacingFinished.ExecuteIfBound();
	BP_SurfacingEnded_CLI();
}

void ABurrowerEnemy::TimelineHideMovement(float Value) 
{
	UpdateBurrowerState(EBurrowerVisibilityState::Hiding);
	GetMesh()->SetRelativeLocation(FMath::Lerp(CachedMeshRelativeLocation, CachedMeshRelativeLocation +
		(FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2)), Value));
}

void ABurrowerEnemy::TimelineHideFinished()
{
	UpdateBurrowerState(EBurrowerVisibilityState::Hidden);
	HidingFinished.ExecuteIfBound();
	HealthComponent->RemoveMark();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	BP_HidingEnded_CLI();
	SetVisibility(true);
	UnsubscribeToSpawnLocation(); 
}
 
void ABurrowerEnemy::SetVisibility(bool isHidden)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Prevent changing hiding state to same
		if (isHidden && VisibilityState != EBurrowerVisibilityState::Hidden)
		{
			UpdateBurrowerState(EBurrowerVisibilityState::Surfacing);
		
		}  else if (!isHidden && VisibilityState == EBurrowerVisibilityState::Hidden)
		{
			UpdateBurrowerState(EBurrowerVisibilityState::Surfacing);
		}
	}
}

// void ABurrowerEnemy::SetAppeared()
// {
// 	MULT_SetBurrowerState(false);
// }
//
// void ABurrowerEnemy::SetDisappeared()
// {
// 	MULT_SetBurrowerState(true);
// }
//  
// void ABurrowerEnemy::SetSurfacingHiding()
// {
// 	MULT_SetBurrowerState(false);
// }

void ABurrowerEnemy::CancelHide()
{
	if (VisibilityState == EBurrowerVisibilityState::Hiding)
	{
		MULT_CancelHideHelper();
	}
}

void ABurrowerEnemy::MULT_CancelHideHelper_Implementation()
{
	HideTimelineComponent->Stop();
	BP_HidingCancelled_CLI();
}

bool ABurrowerEnemy::GetIsSurfaced()
{
	return VisibilityState == EBurrowerVisibilityState::Surfaced;
}

bool ABurrowerEnemy::GetIsSurfacing()
{
	return VisibilityState == EBurrowerVisibilityState::Surfacing;
}

bool ABurrowerEnemy::GetIsHiding()
{
	return VisibilityState == EBurrowerVisibilityState::Hiding;
}

bool ABurrowerEnemy::GetIsHidden()
{
	return VisibilityState == EBurrowerVisibilityState::Hidden;
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

void ABurrowerEnemy::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( ABurrowerEnemy, VisibilityState );
}