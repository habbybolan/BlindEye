// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Health interface for any actor with the HealthComponent
 */
class BLINDEYE_API IHealthInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	float GetHealth();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetHealth(float NewHealth);
};
