// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPooledActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLINDEYE_API IPooledActorInterface
{
	GENERATED_BODY()

public:

	virtual void DisableActor(bool bDisabled) = 0;
	virtual bool GetIsActorDisabled() = 0;
};
