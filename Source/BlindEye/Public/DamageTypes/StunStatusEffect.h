// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "StunStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UStunStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StunDuration;
	
	virtual void ProcessEffect(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
};
