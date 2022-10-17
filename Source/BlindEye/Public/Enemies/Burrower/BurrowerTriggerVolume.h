// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "BurrowerTriggerVolume.generated.h"

enum class EIslandPosition : uint8;

UCLASS()
class BLINDEYE_API ABurrowerTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:

	ABurrowerTriggerVolume();

	UPROPERTY(EditAnywhere)
	EIslandPosition IslandType;
	
};
