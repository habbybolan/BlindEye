// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "SnapperHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API USnapperHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float KnockBackCooldown = 5;

	virtual void KnockBack(FVector KnockBackForce, AActor* DamageCause) override;

protected:
	FTimerHandle KnockBackCooldownTimer;

	bool bCanKnockBack = true;

	void KnockBackOffCooldown();
};
