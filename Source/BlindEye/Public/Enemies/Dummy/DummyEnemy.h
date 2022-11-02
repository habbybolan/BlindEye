// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "DummyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ADummyEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:
	ADummyEnemy(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
};
