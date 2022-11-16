// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemy.h"
#include "DetonatingDummyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ADetonatingDummyEnemy : public ADummyEnemy
{
	GENERATED_BODY()

public:

	ADetonatingDummyEnemy(const FObjectInitializer& ObjectInitializer);
	
};
