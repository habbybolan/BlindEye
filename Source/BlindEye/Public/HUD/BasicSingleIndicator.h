// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HUD/ScreenIndicator.h"
#include "BasicSingleIndicator.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBasicSingleIndicator : public UScreenIndicator
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OffScreenIndicatorType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OnScreenIndicatorType;

	virtual void NativeConstruct() override;
 
protected:

	UPROPERTY()
	UUserWidget* OffScreenIndicator;
	UPROPERTY() 
	UUserWidget* OnScreenIndicator;
	
};
