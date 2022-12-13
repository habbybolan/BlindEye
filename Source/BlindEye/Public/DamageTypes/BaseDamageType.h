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

	UPROPERTY(EditAnywhere)
	bool DebugDamageEverything = false;	// used for debug to allow attacking same team
	
	/**
	 * Process the damage and return the damage multiplier from the hit
	 * @param Owner			Character that caused this attack
	 * @param HitCharacter	Actor that was hit
	 * @param HitLocation	Location the player was hit
	 */
	UFUNCTION()
	virtual float ProcessDamage(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const;
	
};
