// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BurrowerEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/SnapperEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

ABurrowerEnemy::ABurrowerEnemy()
{
	SpawnTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpawnTimeline"));
	HideTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("HideTimeline"));
}

void ABurrowerEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Timeline curve for appearing from the ground
	if (SpawnCurve)
	{
		SpawnUpdateEvent.BindUFunction(this, FName("TimelineSpawnMovement"));
		SpawnFinishedEvent.BindUFunction(this, FName("TimelineSpawnFinished"));
		SpawnTimelineComponent->SetTimelineFinishedFunc(SpawnFinishedEvent);
		SpawnTimelineComponent->AddInterpFloat(SpawnCurve, SpawnUpdateEvent);
	}

	// Timeline curve for hiding the burrower
	if (HideCurve)
	{
		HideUpdateEvent.BindUFunction(this, FName("TimelineHideMovement"));
		HideFinishedEvent.BindUFunction(this, FName("TimelineHideFinished"));
		HideTimelineComponent->SetTimelineFinishedFunc(HideFinishedEvent);
		HideTimelineComponent->AddInterpFloat(HideCurve, HideUpdateEvent);
	}

	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		SpawnedSnappers.Add(world->SpawnActor<ASnapperEnemy>(SnapperType, spawnPoints[randSpawnIndex], GetActorRotation(), params));
		spawnPoints.RemoveAt(randSpawnIndex);
	}

	world->GetTimerManager().SetTimer(HideTimerHandle, this, &ABurrowerEnemy::StartHideLogic, SpawnTimeAppearingLength, false);
}

void ABurrowerEnemy::SpawnAction(FTransform SpawnLocation)
{
	// TODO: Find spawnpoint, teleport to below it so not showing, rise up from ground and spawn enemies
	CachedSpawnLocation = SpawnLocation.GetLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
	SetActorLocation(CachedSpawnLocation);
	SpawnTimelineComponent->PlayFromStart();
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

void ABurrowerEnemy::TimelineSpawnMovement()
{
	float playbackPosition = SpawnTimelineComponent->GetPlaybackPosition();
	SetActorLocation(FMath::Lerp(CachedSpawnLocation, CachedSpawnLocation +
		(FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50)), playbackPosition));
}

void ABurrowerEnemy::TimelineSpawnFinished()
{
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// TODO: play particles and delay resurface
	SpawnSnappers();
}

void ABurrowerEnemy::TimelineHideMovement()
{
	float playbackPosition = HideTimelineComponent->GetPlaybackPosition();
	SetActorLocation(FMath::Lerp(CachedBeforeHidePosition, CachedBeforeHidePosition +
		(FVector::DownVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50)), playbackPosition));
}

void ABurrowerEnemy::TimelineHideFinished()
{
	ActionStateFinished.ExecuteIfBound();
}

void ABurrowerEnemy::StartHideLogic()
{

	CachedBeforeHidePosition = GetActorLocation();
	HideTimelineComponent->PlayFromStart();
}
