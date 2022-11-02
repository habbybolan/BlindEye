// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "DummyEnemyHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UDummyEnemyHealthComponent : public UHealthComponent
{
	GENERATED_BODY()
	
	UDummyEnemyHealthComponent(const FObjectInitializer& ObjectInitializer);

	virtual void DetonateMark() override;
	
};
