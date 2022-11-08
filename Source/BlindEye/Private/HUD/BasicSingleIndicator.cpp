// Copyright (C) Nicholas Johnson 2022


#include "HUD/BasicSingleIndicator.h"

void UBasicSingleIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Target == nullptr) return;

	FVector2D ScreenPosition;
	float Degrees;
	bool IsOnScreen;

	FindScreenEdgeLocationForWorldLocation(ScreenPosition, Degrees, IsOnScreen);

	if (IsOnScreen)
	{
		OffScreenIndicator->SetVisibility(ESlateVisibility::Hidden);
		OnScreenIndicator->SetVisibility(ESlateVisibility::Visible);
		OnScreenIndicator->SetPositionInViewport(ScreenPosition);
	}
	else
	{
		OffScreenIndicator->SetVisibility(ESlateVisibility::Visible);
		OnScreenIndicator->SetVisibility(ESlateVisibility::Hidden);
		OffScreenIndicator->SetPositionInViewport(ScreenPosition);
		OffScreenIndicator->SetRenderTransformAngle(Degrees);
	}
}

void UBasicSingleIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	OffScreenIndicator = CreateWidgetInstance(*this, OffScreenIndicatorType, FName("OffScreenIndicator"));
	OffScreenIndicator->AddToViewport();
	OnScreenIndicator = CreateWidgetInstance(*this, OnScreenIndicatorType, FName("OnScreenIndicator"));
	OnScreenIndicator->AddToViewport();
}
