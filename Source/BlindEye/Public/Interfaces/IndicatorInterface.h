// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IndicatorInterface.generated.h"

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UIndicatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLINDEYE_API IIndicatorInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual FVector GetIndicatorPosition() = 0;
};
