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

UENUM()
enum class TEAMS
{
	Player = 0,
	Enemy = 1
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TEAMS GetTeam();
};
