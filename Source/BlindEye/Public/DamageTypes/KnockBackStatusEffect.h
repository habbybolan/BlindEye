// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "KnockBackStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UKnockBackStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float KnockBackForce = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float KnockUpForce = 200.f;
	
	virtual void ProcessEffect(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
};
