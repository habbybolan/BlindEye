// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void SetTarget(AActor* target);

	UPROPERTY(EditDefaultsOnly)
	float EdgePercent = 0.9;
	
protected:

	UPROPERTY()
	AActor* Target;

	UFUNCTION(BlueprintCallable)
	void FindScreenEdgeLocationForWorldLocation(FVector2D& OutScreenPosition,
		float& OutRotationAngleDegrees, bool &bIsOnScreen);
};
