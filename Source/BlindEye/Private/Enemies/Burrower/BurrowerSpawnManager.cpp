// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnManager.h"

#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABurrowerSpawnManager::ABurrowerSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABurrowerSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	// Only run manager logic on the server
	if (GetLocalRole() == ROLE_Authority)
	{
		CacheSpawnPoints();

		UWorld* world = GetWorld();
		if (!world) return;

		world->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABurrowerSpawnManager::SpawnBurrower, SpawnDelay, true, 0.1);
	}
}

void ABurrowerSpawnManager::OnBurrowerDeath(AActor* BurrowerActor)
{
	SpawnedBurrowers.Remove(BurrowerActor->GetUniqueID());
}

void ABurrowerSpawnManager::SpawnBurrower()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FTransform SpawnPoint = FindRandomSpawnPoint();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 
	ABurrowerEnemy* SpawnedBurrower = world->SpawnActor<ABurrowerEnemy>(BurrowerType, SpawnPoint.GetLocation(), SpawnPoint.Rotator(), params);
	if (SpawnedBurrower)
	{
		SpawnedBurrowers.Add(SpawnedBurrower->GetUniqueID(), MakeWeakObjectPtr(SpawnedBurrower));
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SpawnedBurrower))
		{
			HealthInterface->GetHealthComponent()->OnDeathDelegate.AddUFunction(this, FName("OnBurrowerDeath"));
		}
	}
}

FTransform ABurrowerSpawnManager::FindRandomSpawnPoint()
{
	if (SpawnLocation.Num() == 0) return FTransform();
	uint32 randIndex = UKismetMathLibrary::RandomInteger(SpawnLocation.Num());
	return SpawnLocation[randIndex]->GetTransform();
}

void ABurrowerSpawnManager::CacheSpawnPoints()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerSpawnPoint::StaticClass(), SpawnLocation);
}

