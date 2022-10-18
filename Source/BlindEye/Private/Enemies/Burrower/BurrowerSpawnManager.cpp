// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnManager.h"

#include "AIController.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABurrowerSpawnManager::ABurrowerSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bNetLoadOnClient = false;
}

// Called when the game starts or when spawned
void ABurrowerSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaps();
	CacheSpawnPoints();

	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABurrowerSpawnManager::SpawnBurrower, SpawnDelay, true, 0.1);

	// Find all trigger volumes and subscribe to their events
	TArray<AActor*> OutTriggerVolumes;
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerTriggerVolume::StaticClass(), OutTriggerVolumes);
	for (AActor* VolumeActor : OutTriggerVolumes)
	{
		ABurrowerTriggerVolume* BurrowerVolume = Cast<ABurrowerTriggerVolume>(VolumeActor);
		BurrowerTriggerVolumes.Add(BurrowerVolume->IslandType, BurrowerVolume);
		BurrowerVolume->OnActorBeginOverlap.AddDynamic(this, &ABurrowerSpawnManager::TriggerVolumeOverlapped);
		BurrowerVolume->CustomOverlapDelegate.AddUFunction(this, TEXT("TriggerVolumeLeft"));
	}
}

void ABurrowerSpawnManager::InitializeMaps()
{
	// Initialize Map enum keys to empty arrays
	for (uint8 i = 0; i < (uint8)EIslandPosition::Count; i++)
	{
		EIslandPosition position = static_cast<EIslandPosition>(i);
		SpawnedBurrowers.Add(position, TArray<ABurrowerEnemy*>());
		BurrowerSpawnPoints.Add(position, TArray<ABurrowerSpawnPoint*>());
		
	}
}

void ABurrowerSpawnManager::OnBurrowerDeath(AActor* BurrowerToDelete)
{ 
	for (TPair<EIslandPosition, TArray<ABurrowerEnemy*>>& pair : SpawnedBurrowers)
	{
		uint8 index = 0;
		for (ABurrowerEnemy* Burrower : pair.Value)
		{
			if (BurrowerToDelete == Burrower)
			{
				pair.Value.RemoveAt(index);
				return;
			}
			index++;
		}
	}
	//SpawnedBurrowers.Remove(BurrowerActor->GetUniqueID());
}

void ABurrowerSpawnManager::SpawnBurrower()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	ABurrowerSpawnPoint* SpawnPoint = FindRandomSpawnPoint();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 
	ABurrowerEnemy* SpawnedBurrower = world->SpawnActor<ABurrowerEnemy>(BurrowerType, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), params);
	if (SpawnedBurrower)
	{
		SpawnedBurrower->SpawnMangerSetup(SpawnPoint->IslandType, this);
		SpawnedBurrowers[SpawnPoint->IslandType].Add(SpawnedBurrower);
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedBurrower))
		{
			HealthInterface->GetHealthComponent()->OnDeathDelegate.AddUFunction(this, FName("OnBurrowerDeath"));
		}
	}
}

TArray<ABlindEyePlayerCharacter*> ABurrowerSpawnManager::GetPlayersOnIsland(EIslandPosition IslandType)
{
	ABurrowerTriggerVolume* Trigger = BurrowerTriggerVolumes[IslandType];
	TArray<ABlindEyePlayerCharacter*> PlayersInTrigger = Trigger->GetPlayerActorsOverlapping();
	return PlayersInTrigger;
}

ABurrowerSpawnPoint* ABurrowerSpawnManager::FindRandomSpawnPoint()
{
	// TODO: Way to check when last island added
	uint8 randIndexIsland = UKismetMathLibrary::RandomInteger((uint8)EIslandPosition::Count - 1);
	EIslandPosition type = static_cast<EIslandPosition>(randIndexIsland);
	uint8 randIndexSpawnPoint = UKismetMathLibrary::RandomInteger(BurrowerSpawnPoints[type].Num());
	ABurrowerSpawnPoint* RandSpawnPoint = BurrowerSpawnPoints[type][randIndexSpawnPoint];
	
	return RandSpawnPoint;
}

void ABurrowerSpawnManager::CacheSpawnPoints()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerSpawnPoint::StaticClass(), SpawnPoints);
	for (AActor* SpawnPoint : SpawnPoints)
	{
		ABurrowerSpawnPoint* BurrowerSpawnPoint = Cast<ABurrowerSpawnPoint>(SpawnPoint);
		BurrowerSpawnPoints[BurrowerSpawnPoint->IslandType].Add(BurrowerSpawnPoint);
	}
}

void ABurrowerSpawnManager::TriggerVolumeOverlapped(AActor* OverlappedActor, AActor* OtherActor)
{
	// Check if it is a player that entered the island
	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		ABurrowerTriggerVolume* BurrowerVolume = Cast<ABurrowerTriggerVolume>(OverlappedActor);
		PlayerEnteredIsland(BlindEyePlayerCharacter, BurrowerVolume->IslandType);
	}
}

void ABurrowerSpawnManager::TriggerVolumeLeft(AActor* EndOverlappedActor, AActor* OtherActor)
{
	// Check if it is a player that left the island
	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		ABurrowerTriggerVolume* BurrowerVolume = Cast<ABurrowerTriggerVolume>(EndOverlappedActor);
		PlayerLeftIsland(BlindEyePlayerCharacter, BurrowerVolume->IslandType);
	}
}

void ABurrowerSpawnManager::PlayerEnteredIsland(ABlindEyePlayerCharacter* Player, EIslandPosition IslandType)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 4.0f, FColor::Cyan, "Player entered island: " + UEnum::GetValueAsString(IslandType));
	for (ABurrowerEnemy* Burrower : SpawnedBurrowers[IslandType])
	{
		AController* Controller =  Burrower->GetController();
		if (Controller)
		{
			ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
			BurrowerController->NotifyPlayerEnteredIsland(Player);
		}
	}
}

void ABurrowerSpawnManager::PlayerLeftIsland(ABlindEyePlayerCharacter* Player, EIslandPosition IslandType)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 4.0f, FColor::Cyan, "Player left island: " + UEnum::GetValueAsString(IslandType));
	for (ABurrowerEnemy* Burrower : SpawnedBurrowers[IslandType])
	{
		AController* Controller =  Burrower->GetController();
		if (Controller)
		{
			ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
			BurrowerController->NotifyPlayerLeftIsland(Player);
		}
	}
}

