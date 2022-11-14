// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
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
	
	virtual void SetTarget(UObject* target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UObject* GetTarget(); 

	UPROPERTY(EditDefaultsOnly)
	float EdgePercent = 0.9;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UCanvasPanel* IndicatorContainer;

	virtual void RemoveFromParent() override;
	
protected: 
	
	UObject* Target = nullptr;

	UFUNCTION(BlueprintCallable)
	void FindScreenEdgeLocationForWorldLocation(FVector2D& OutScreenPosition,
		float& OutRotationAngleDegrees, bool &bIsOnScreen);

	void NormalizeScreenCoordinates(FVector2D& OutScreenPosition, const FVector2D ViewportSiz);
};
