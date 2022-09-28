// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityUserInterface.generated.h"

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UAbilityUserInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLINDEYE_API IAbilityUserInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual bool TryConsumeBirdMeter(float PercentAmount) = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetBirdMeter() = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetBirdMeterPercent() = 0; 

	
};
