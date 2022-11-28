// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenIndicatorCustomRotationIcon.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UScreenIndicatorCustomRotationIcon : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyRotation(float Degrees);
	
};
