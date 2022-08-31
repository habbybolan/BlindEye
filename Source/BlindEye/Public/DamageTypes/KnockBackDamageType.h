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
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation) const override;
};
