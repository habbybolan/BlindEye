// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "BurrowerHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBurrowerHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

protected:

	virtual void KnockBack(FVector KnockBackForce, AActor* DamageCause) override;
	
};
