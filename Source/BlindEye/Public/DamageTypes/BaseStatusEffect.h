// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.generated.h"

class UHealthComponent;

/**
 * Contained in BaseDamageType, deals with any status effects applied to player.
 */
UCLASS(Abstract, Blueprintable)
class BLINDEYE_API UBaseStatusEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
	PURE_VIRTUAL(UBaseStatusEffect::ProcessEffect, );
};
