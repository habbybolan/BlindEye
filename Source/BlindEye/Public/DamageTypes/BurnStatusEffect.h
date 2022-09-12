// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "BurnStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBurnStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagePerSec;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration;

	virtual void ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
