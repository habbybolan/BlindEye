// Copyright (C) Nicholas Johnson 2022


#include "Tools/LocalPlayerSubsystem_Pooling.h"

void ULocalPlayerSubsystem_Pooling::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UObject* DataTableObj = StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Game/Blueprints/Player/Flock/DT_ActorPooler"));
	if (DataTableObj == nullptr) return;
	
	UDataTable* DT = Cast<UDataTable>(DataTableObj);
	//static ConstructorHelpers::FObjectFinder<UDataTable> DataTableObj(TEXT("/Game/Blueprints/Player/Flock/DT_ActorPooler"));

	// retrieve all struct objects from DataTable
	TArray<FActorPooler*> OutArray;
	if (DT != nullptr)
	{
		DT->GetAllRows<FActorPooler>(TEXT("ActorPoolerDT"), OutArray);
		for (FActorPooler* StructPool : OutArray)
		{
			ItemsToPool.Add(*StructPool);
		}
	}

	SetupPooledActors();
}
 
void ULocalPlayerSubsystem_Pooling::Deinitialize()
{
	Super::Deinitialize();
}

AActor* ULocalPlayerSubsystem_Pooling::GetPooledActor(EActorPoolType tag)
{
	TArray<AActor*> PooledActorsOfType = PooledActors[tag];

	// Find A free pooled actor to use
	for (uint8 i = 0; i < PooledActorsOfType.Num(); i++)
	{
		if (PooledActorsOfType[i]->IsHidden())
		{
			PooledActorsOfType[i]->SetActorHiddenInGame(false);
			return PooledActorsOfType[i];
		}
	}
	
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	
	// Otherwise, if the pool can expand, create a new actor in the pool
	for (FActorPooler Item : ItemsToPool)
	{
		if (Item.Tag == tag && Item.bShouldExpand)
		{
			AActor* NewActor = CreateNewPoolItem(Item);
			NewActor->SetActorHiddenInGame(false);
			return NewActor;
		}
	}
	return nullptr;
}

AActor* ULocalPlayerSubsystem_Pooling::CreateNewPoolItem(FActorPooler Item)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// Add a new actor to pool
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* NewActor = World->SpawnActor<AActor>(Item.ObjectToPool, FVector::ZeroVector, FRotator::ZeroRotator, params);
	if (NewActor)
	{
		NewActor->SetActorHiddenInGame(true);
		PooledActors[Item.Tag].Add(NewActor);
		return NewActor;
	}
	return nullptr;
}

void ULocalPlayerSubsystem_Pooling::ReturnActorToPool(AActor* ActorToReturn)
{
	ActorToReturn->SetActorHiddenInGame(true);
}

void ULocalPlayerSubsystem_Pooling::SetupPooledActors()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Loop through all items to pool and spawn their actor types for pooling
	for (FActorPooler item : ItemsToPool)
	{
		PooledActors.Add(item.Tag, TArray<AActor*>());
		for (uint8 i = 0; i < item.AmountToPool; i++)
		{
			CreateNewPoolItem(item);
		}
	}
}
