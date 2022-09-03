// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "BurnDamageType.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBurnDamageType : public UBaseDamageType
{
	GENERATED_BODY()

public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagePerSec;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration;
	
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
