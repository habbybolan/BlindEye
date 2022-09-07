// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BurrowerEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/SnapperEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

ABurrowerEnemy::ABurrowerEnemy()
{
	SpawnTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpawnTimeline"));
}

void ABurrowerEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnCurve)
	{
		SpawnUpdateEvent.BindUFunction(this, FName("TimelineSpawnMovement"));
		SpawnFinishedEvent.BindUFunction(this, FName("TimelineSpawnFinished"));
		//SpawnTimelineComponent->SetTimelinePostUpdateFunc(SpawnUpdateEvent);
		SpawnTimelineComponent->SetTimelineFinishedFunc(SpawnFinishedEvent);
		
		SpawnTimelineComponent->AddInterpFloat(SpawnCurve, SpawnUpdateEvent);
	}

	GetCapsuleComponent()->SetEnableGravity(0);
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
}

void ABurrowerEnemy::SpawnAction(FTransform SpawnLocation)
{
	// TODO: Find spawnpoint, teleport to below it so not showing, rise up from ground and spawn enemies
	CachedSpawnLocation = SpawnLocation.GetLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
	SetActorLocation(CachedSpawnLocation);
	SpawnTimelineComponent->PlayFromStart();
}

void ABurrowerEnemy::AttackAction()
{
	// TODO:
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
	SetActorLocation(FMath::Lerp(CachedSpawnLocation, CachedSpawnLocation + (FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50), playbackPosition));
}

void ABurrowerEnemy::TimelineSpawnFinished()
{
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// TODO: Spawn snappers
}
