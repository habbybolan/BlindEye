// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseStatusEffect.h"
#include "TauntStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UTauntStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration = 3.0f;

	virtual void ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
