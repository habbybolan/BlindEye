// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityUserInterface.generated.h"

UINTERFACE(MinimalAPI)
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool TryConsumeBirdMeter(float PercentAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetBirdMeter();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetBirdMeterPercent(); 

	
};
