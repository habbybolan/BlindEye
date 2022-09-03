// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "KnockBackDamageType.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UKnockBackDamageType : public UBaseDamageType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float KnockBackForce = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float KnockUpForce = 200.f;
	
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
};
