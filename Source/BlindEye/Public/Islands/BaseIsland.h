// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseIsland.generated.h"

class UBurrowerTriggerVolume;
UCLASS(Abstract)
class BLINDEYE_API ABaseIsland : public AActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseIsland();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* BaseIslandMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) 
	UStaticMeshComponent* Shield;

	UPROPERTY(EditDefaultsOnly)
	UBurrowerTriggerVolume* IslandTrigger;

	uint8 IslandID;

	virtual void Initialize(uint8 islandID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
