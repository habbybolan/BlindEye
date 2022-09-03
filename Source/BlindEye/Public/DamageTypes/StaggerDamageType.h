// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "StaggerDamageType.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UStaggerDamageType : public UBaseDamageType
{
	GENERATED_BODY()

public:
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
