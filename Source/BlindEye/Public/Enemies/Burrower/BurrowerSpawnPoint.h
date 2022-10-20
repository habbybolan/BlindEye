// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerSpawnPoint.generated.h"

enum class EIslandPosition : uint8;

UCLASS()
class BLINDEYE_API ABurrowerSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	EIslandPosition IslandType;

};
