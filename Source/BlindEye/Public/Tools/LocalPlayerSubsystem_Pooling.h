// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LocalPlayerSubsystem_Pooling.generated.h"

UENUM(BlueprintType)
enum class EActorPoolType : uint8
{
	// values correspond to row index of ActorPool DataTable
	CrowBoid,
	PhoenixBoid,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FActorPooler : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EActorPoolType Tag;
	UPROPERTY(EditAnywhere)
	uint8 AmountToPool;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ObjectToPool;
	UPROPERTY(EditAnywhere)
	bool bShouldExpand;
};

/**
 * Pooled Subsystem for each local player
 */
UCLASS(DisplayName=PoolingSubsystem)
class BLINDEYE_API ULocalPlayerSubsystem_Pooling : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FActorPooler> ItemsToPool;

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	AActor* GetPooledActor(EActorPoolType tag);

	// Create a new pooled actor
	AActor* CreateNewPoolItem(FActorPooler Item);

	void ReturnActorToPool(AActor* ActorToReturn);

private:

	TMap<EActorPoolType, TArray<AActor*>> PooledActors;
	
	// Creates the initial amount of actors to pool
	void SetupPooledActors();

	const FString ActorPoolDTLocation = TEXT("/Game/Blueprints/Player/Flock/DT_ActorPooler");
};
