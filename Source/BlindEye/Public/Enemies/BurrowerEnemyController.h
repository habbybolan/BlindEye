// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "BurrowerEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABurrowerEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
protected:
	FTimerHandle SpawnTimerHandle;

	void SpawnEnemies();
	
};
