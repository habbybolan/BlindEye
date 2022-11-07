// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnManager.h"

#include "AIController.h"
#include "Islands/Island.h"
#include "Islands/IslandManager.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

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

	UWorld* World = GetWorld();
	if (World)
	{
		// Initialize Burrower spawner once game set as started
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		BlindEyeGS->GameStartedDelegate.AddDynamic(this, &ABurrowerSpawnManager::OnGameStarted);
	}
}

void ABurrowerSpawnManager::OnGameStarted()
{
	Initialize();
}

void ABurrowerSpawnManager::Initialize()
{
	InitializeMaps();

	UWorld* world = GetWorld();
	if (!world) return;
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(world));
	check(BlindEyeGS)
	IslandManager = BlindEyeGS->GetIslandManager();
	check(IslandManager)

	IslandManager->IslandAddedDelegate.AddDynamic(this, &ABurrowerSpawnManager::NewIslandAdded);
	
	// Subscribe to all delegate volume triggers on the islands
	for (AIsland* Island : IslandManager->GetActiveIslands() )
	{
		SubscribeToIsland(Island);
	}
}

void ABurrowerSpawnManager::InitializeMaps()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		// Initialize Map enum keys to empty arrays
		TArray<AIsland*> Islands = BlindEyeGS->GetIslandManager()->GetActiveIslands();
		for (uint8 i = 0; i < Islands.Num(); i++)
		{
			SpawnedBurrowers.Add(Islands[i]->IslandID, TArray<ABurrowerEnemy*>());
		}
	}
}

void ABurrowerSpawnManager::NewIslandAdded(AIsland* Island)
{
	SubscribeToIsland(Island);
	SpawnedBurrowers.Add(Island->IslandID, TArray<ABurrowerEnemy*>());
}

void ABurrowerSpawnManager::SubscribeToIsland(AIsland* Island)
{
	Island->IslandTrigger->CustomOverlapStartDelegate.AddDynamic(this, &ABurrowerSpawnManager::TriggerVolumeOverlapped);
	Island->IslandTrigger->CustomOverlapEndDelegate.AddDynamic(this, &ABurrowerSpawnManager::TriggerVolumeLeft);
}

void ABurrowerSpawnManager::OnBurrowerDeath(AActor* BurrowerToDelete)
{ 
	for (TPair<uint8, TArray<ABurrowerEnemy*>>& pair : SpawnedBurrowers)
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
}

void ABurrowerSpawnManager::SpawnBurrowerRandLocation()
{
	// Find an island without max number of burrowers
	for (AIsland* Island : IslandManager->GetActiveIslands())
	{
		if (SpawnedBurrowers[Island->IslandID].Num() < MaxNumBurrowersPerIsland)
		{
			UBurrowerSpawnPoint* SpawnPoint = Island->GetRandUnusedBurrowerSpawnPoint();
			if (SpawnPoint)
			{
				SpawnBurrowerHelper(SpawnPoint, Island);
				return;
			}
		}
	}
}

void ABurrowerSpawnManager::SpawnBurrower(AIsland* Island)
{
	UBurrowerSpawnPoint* SpawnPoint = Island->GetRandUnusedBurrowerSpawnPoint();
	if (SpawnPoint)
	{
		SpawnBurrowerHelper(SpawnPoint, Island);
	}
}

void ABurrowerSpawnManager::SpawnBurrowerHelper(UBurrowerSpawnPoint* SpawnPoint, AIsland* Island)
{
	// prevent more spawning if max num burrowers already spawned
	if (SpawnedBurrowers[Island->IslandID].Num() >= MaxNumBurrowersPerIsland) return;
	
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 
	ABurrowerEnemy* SpawnedBurrower = world->SpawnActor<ABurrowerEnemy>(BurrowerType, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), params);
	if (SpawnedBurrower)
	{
		SpawnedBurrower->SpawnMangerSetup(SpawnPoint->IslandID, this);
		SpawnedBurrower->SubscribeToSpawnLocation(SpawnPoint);
		SpawnedBurrower->SubscribeToIsland(Island);
		SpawnedBurrowers[SpawnPoint->IslandID].Add(SpawnedBurrower);
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedBurrower))
		{
			HealthInterface->GetHealthComponent()->OnDeathDelegate.AddDynamic(this, &ABurrowerSpawnManager::OnBurrowerDeath);
		}
	}
}

TArray<ABlindEyePlayerCharacter*> ABurrowerSpawnManager::GetPlayersOnIsland(uint8 islandID)
{
	AIsland* Island = IslandManager->GetIslandOfID(islandID);
	check(Island)

	TArray<ABlindEyePlayerCharacter*> PlayersInTrigger = Island->IslandTrigger->GetPlayerActorsOverlapping();
	return PlayersInTrigger;
}

void ABurrowerSpawnManager::SetInBurstState()
{
	for (TPair<uint8, TArray<ABurrowerEnemy*>> Pair : SpawnedBurrowers)
	{
		for (ABurrowerEnemy* Burrower : Pair.Value)
		{
			ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Burrower->GetController());
			check(BurrowerController);

			BurrowerController->SetInBurstState();
		}
		
	}
}

UBurrowerSpawnPoint* ABurrowerSpawnManager::FindRandomUnusedSpawnPoint()
{
	AIsland* RandIsland = IslandManager->GetRandIsland();
	return RandIsland->GetRandUnusedBurrowerSpawnPoint();
}

void ABurrowerSpawnManager::TriggerVolumeOverlapped(UPrimitiveComponent* OverlappedActor, AActor* OtherActor)
{
	// Check if it is a player that entered the island
	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		UBurrowerTriggerVolume* BurrowerVolume = Cast<UBurrowerTriggerVolume>(OverlappedActor);
		PlayerEnteredIsland(BlindEyePlayerCharacter, BurrowerVolume->IslandID);
	}
}

void ABurrowerSpawnManager::TriggerVolumeLeft(UPrimitiveComponent* EndOverlappedActor, AActor* OtherActor)
{
	// Check if it is a player that left the island
	if (ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
 		UBurrowerTriggerVolume* BurrowerVolume = Cast<UBurrowerTriggerVolume>(EndOverlappedActor);
		PlayerLeftIsland(BlindEyePlayerCharacter, BurrowerVolume->IslandID);
	}
}
 
void ABurrowerSpawnManager::PlayerEnteredIsland(ABlindEyePlayerCharacter* Player, uint8 islandID)
{
	if (SpawnedBurrowers.Contains(islandID))
	{
		for (ABurrowerEnemy* Burrower : SpawnedBurrowers[islandID])
		{
			AController* Controller =  Burrower->GetController();
			if (Controller)
			{
				ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
				BurrowerController->NotifyPlayerEnteredIsland(Player);
			}
		}
	}
}

void ABurrowerSpawnManager::PlayerLeftIsland(ABlindEyePlayerCharacter* Player, uint8 islandID)
{
	if (SpawnedBurrowers.Contains(islandID))
	{
		for (ABurrowerEnemy* Burrower : SpawnedBurrowers[islandID])
		{
			AController* Controller =  Burrower->GetController();
			if (Controller)
			{
				ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
				BurrowerController->NotifyPlayerLeftIsland(Player);
			}
		}
	}
}

