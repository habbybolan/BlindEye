// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IslandSpawnPoint.generated.h"

UCLASS()
class BLINDEYE_API AIslandSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AIslandSpawnPoint();

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Root;

protected:
	virtual void BeginPlay() override;

};
