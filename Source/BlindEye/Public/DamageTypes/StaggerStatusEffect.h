// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "StaggerStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UStaggerStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public:
	
	virtual void ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
};
