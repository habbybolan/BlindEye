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

class UHealthComponent;
class UMarkerComponent;

/**
 * Health interface for any actor with a HealthComponent
 * Used by the HealthComponent, or by other classes to get info on HealthComponent
 */
class BLINDEYE_API IHealthInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	float GetHealth();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	float GetMaxHealth();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	float GetHealthPercent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TEAMS GetTeam();

	// Any extra specific logic, outside of damage logic, for actor to perform on taking damage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UHealthComponent* GetHealthComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnDeath();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetIsDead();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UMarkerComponent* GetMarkerComponent();
};
