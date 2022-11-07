// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IndicatorInterface.h"
#include "ScreenIndicator.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UScreenIndicator : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTarget(TScriptInterface<IIndicatorInterface> target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TScriptInterface<IIndicatorInterface> GetTarget(); 

	UPROPERTY(EditDefaultsOnly)
	float EdgePercent = 0.9;
	
protected: 

	UPROPERTY()
	TScriptInterface<IIndicatorInterface> Target;

	UFUNCTION(BlueprintCallable)
	void FindScreenEdgeLocationForWorldLocation(FVector2D& OutScreenPosition,
		float& OutRotationAngleDegrees, bool &bIsOnScreen);
};
