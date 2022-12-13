// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
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

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() = 0;
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() = 0;
	UFUNCTION(BlueprintCallable) 
	virtual void SetHealth(float NewHealth) = 0;
	UFUNCTION(BlueprintCallable)
	virtual float GetHealthPercent() = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetMass() = 0;

	UFUNCTION(BlueprintCallable)
	virtual TEAMS GetTeam() = 0;

	// Any extra specific logic, outside of damage logic, for actor to perform on taking damage
	UFUNCTION(BlueprintCallable) 
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) = 0;

	UFUNCTION(BlueprintCallable)
	virtual UHealthComponent* GetHealthComponent() = 0;
  
	UFUNCTION(BlueprintCallable)
	virtual void OnDeath(AActor* ActorThatKilled) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsDead() = 0;

	UFUNCTION(BlueprintCallable)
	virtual UMarkerComponent* GetMarkerComponent() = 0;
};
