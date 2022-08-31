// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "StunDamageType.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UStunDamageType : public UBaseDamageType
{
	GENERATED_BODY()

public:
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation) const override;
};
