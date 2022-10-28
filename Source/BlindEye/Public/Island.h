// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Island.generated.h"

UCLASS()
class BLINDEYE_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

	UPROPERTY(EditDefaultsOnly)
	ABurrowerTriggerVolume* BurrowerTriggerVolume;

	UPROPERTY(EditDefaultsOnly)
	TArray<UChildActorComponent*> BurrowerSpawnPoints;

	uint8 IslandID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
