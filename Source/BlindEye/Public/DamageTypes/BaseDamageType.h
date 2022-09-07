// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "BaseDamageType.generated.h"

class UBaseStatusEffect;
class UHealthComponent;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UBaseDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	UBaseDamageType();
	~UBaseDamageType() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UBaseStatusEffect>> StatusEffects;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=( ClampMin=0 ))
	float DamageMultiplier = 1;
	
	/**
	 * Process the damage and return the damage multiplier from the hit
	 * @param Owner			Character that caused this attack
	 * @param HitCharacter	Player Character that was hit
	 * @param HitLocation	Location the player was hit
	 */
	UFUNCTION()
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const;
	
};