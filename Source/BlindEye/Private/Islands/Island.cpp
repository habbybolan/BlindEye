// Copyright (C) Nicholas Johnson 2022


#include "Islands/Island.h"

#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BurrowerSpawnPoint = CreateDefaultSubobject<UBurrowerSpawnPoint>("Burrower Spawn Point");
	BurrowerSpawnPoint->SetupAttachment(RootComponent);
}

void AIsland::BeginPlay()
{
	Super::BeginPlay();

	EndLocation = GetActorLocation();
	if (bActive == false)
	{
		Disable(true);
	}
}

void AIsland::Initialize(uint8 islandID)
{
	Super::Initialize(islandID);
	TSet<UActorComponent*> AllComponents = GetComponents();
	for (auto ChildActor : AllComponents)
	{
		// cache all owned burrower spawn points
		if (UBurrowerSpawnPoint* BSpawnPoint = Cast<UBurrowerSpawnPoint>(ChildActor))
		{
			BSpawnPoint->IslandID = islandID;
			OwnedBurrowerSpawnPoints.Add(BSpawnPoint);
		}
	}
}

TArray<UBurrowerSpawnPoint*> AIsland::GetBurrowerSpawnPoints()
{
	return OwnedBurrowerSpawnPoints;
}

void AIsland::MULT_SpawnIsland_Implementation(FVector startLocation)
{
	if (bActive) return; 

	StartLocation = startLocation;
	SetActorLocation(StartLocation);
	bSpawning = true;

	// TODO: Notify BP to start transition
	Disable(false);

	BP_StartLevelMovement();
}

bool AIsland::GetIsActive()
{
	return bActive;
}

UBurrowerSpawnPoint* AIsland::GetRandUnusedBurrowerSpawnPoint()
{
	uint8 randIndexSpawnPoint = UKismetMathLibrary::RandomInteger(OwnedBurrowerSpawnPoints.Num());
	
	UBurrowerSpawnPoint* RandSpawnPoint = GetBurrowerSpawnPoints()[randIndexSpawnPoint];
	// if random spawn point not valid
	if (RandSpawnPoint->bInUse)
	{
		uint8 CheckedCount = 1;
		// Loop through all spawn points, starting from rand index to find valid spawn point
		while (CheckedCount <= OwnedBurrowerSpawnPoints.Num())
		{
			randIndexSpawnPoint++;
			// Loops back around if reached end of spawn point array
			if (randIndexSpawnPoint >= OwnedBurrowerSpawnPoints.Num())
			{
				randIndexSpawnPoint = 0;
			}
			// Check if next spawn point is valid
			UBurrowerSpawnPoint* SpawnPoint = OwnedBurrowerSpawnPoints[randIndexSpawnPoint];
			if (!SpawnPoint->bInUse)
			{
				return SpawnPoint;
			}
			CheckedCount++;
		}
	} else
	{
		return RandSpawnPoint;
	}

	// There should always be enough spawn points on an island for the burrowers (Num spawn points >= max num burrowers)
	return nullptr;
}

void AIsland::IslandFinishedSpawning()
{
	SpawnFinishedDelegate.Broadcast(this);
	bSpawning = false;
	bActive = true;
}

void AIsland::Disable(bool bDisabled)
{
	for (UActorComponent* Component : GetComponents())
	{
		if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Component))
		{
			Mesh->SetHiddenInGame(bDisabled);
			if (bDisabled)
			{
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				SetActorLocation(FVector::ZeroVector);
				IslandTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			} else
			{
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				IslandTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			
		}
	}
}

