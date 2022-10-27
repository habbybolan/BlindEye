// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "HunterHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UHunterHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

	virtual void KnockBack(FVector KnockBackForce, AActor* DamageCause) override;

	virtual float ProcessDamage(float Damage) override;
};
