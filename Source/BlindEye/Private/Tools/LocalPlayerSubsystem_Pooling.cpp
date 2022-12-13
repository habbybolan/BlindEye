// Copyright (C) Nicholas Johnson 2022


#include "Tools/LocalPlayerSubsystem_Pooling.h"

#include "Interfaces/PooledActorInterface.h"

void ULocalPlayerSubsystem_Pooling::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// UObject* DataTableObj = StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Game/Blueprints/Player/Flock/DT_ActorPooler"));
	// if (DataTableObj == nullptr) return;
	//
	// UDataTable* DT = Cast<UDataTable>(DataTableObj);
	//
	// // retrieve all struct objects from DataTable
	// TArray<FActorPooler*> OutArray;
	// if (DT != nullptr)
	// {
	// 	DT->GetAllRows<FActorPooler>(TEXT("ActorPoolerDT"), OutArray);
	// 	for (FActorPooler* StructPool : OutArray)
	// 	{
	// 		ItemsToPool.Add(*StructPool);
	// 	}
	// }
	//
	// SetupPooledActors();
}
 
void ULocalPlayerSubsystem_Pooling::Deinitialize()
{
	Super::Deinitialize();
}

AActor* ULocalPlayerSubsystem_Pooling::GetPooledActor(EActorPoolType tag)
{
	// in case the Map does not contain the pooled objects associated with the tag
	if (!PooledActors.Contains(tag))
	{
		FString TagName = UEnum::GetValueAsString(tag);
		UE_LOG(LogTemp, Error, TEXT("%s tag was not added to pool. Either was not added to Pooled DataTable or Actor did not implement IPooledActorInterface"), *(TagName));
		return nullptr;
	}
	
	TArray<AActor*> PooledActorsOfType = PooledActors[tag];

	// Find A free pooled actor to use
	for (uint8 i = 0; i < PooledActorsOfType.Num(); i++)
	{
		if (GetIsActorDisabled(PooledActorsOfType[i]))
		{
			SetActorDisabled(PooledActorsOfType[i], false);
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
			SetActorDisabled(NewActor, false);
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
		PooledActors[Item.Tag].Add(NewActor);
		ReturnActorToPool(NewActor);
		return NewActor;
	}
	return nullptr;
}

void ULocalPlayerSubsystem_Pooling::ReturnActorToPool(AActor* ActorToReturn)
{
	SetActorDisabled(ActorToReturn, true);
}

void ULocalPlayerSubsystem_Pooling::SetupPooledActors()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Loop through all items to pool and spawn their actor types for pooling
	for (FActorPooler item : ItemsToPool)
	{
		// if object does not implemented pool interface, dont allow it
		if (!item.ObjectToPool->ImplementsInterface(UPooledActorInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("%s does not implement IPooledActorInterface"), *item.ObjectToPool->GetName())
			continue;
		}

		// Otherwise, setup initial pool
		PooledActors.Add(item.Tag, TArray<AActor*>());
		for (uint8 i = 0; i < item.AmountToPool; i++)
		{
			CreateNewPoolItem(item);
		}
	}
}

void ULocalPlayerSubsystem_Pooling::SetActorDisabled(AActor* ActorToReturn, bool bDisableActor)
{
	if (IPooledActorInterface* PooledActor = Cast<IPooledActorInterface>(ActorToReturn))
	{
		PooledActor->DisableActor(bDisableActor);
	}
}

bool ULocalPlayerSubsystem_Pooling::GetIsActorDisabled(AActor* Actor)
{
	if (IPooledActorInterface* PooledActor = Cast<IPooledActorInterface>(Actor))
	{
		return PooledActor->GetIsActorDisabled();
	}
	return true;
}
