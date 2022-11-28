// Copyright (C) Nicholas Johnson 2022


#include "HUD/BasicSingleIndicator.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

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
		UWidgetLayoutLibrary::SlotAsCanvasSlot(OnScreenIndicator)->SetPosition(ScreenPosition);
	}
	else
	{
		OffScreenIndicator->SetVisibility(ESlateVisibility::Visible);
		OnScreenIndicator->SetVisibility(ESlateVisibility::Hidden);
		UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(OffScreenIndicator);
		CanvasSlot->SetPosition(ScreenPosition - FVector2D(1, 0) * CanvasSlot->GetSize().X / 2);
		OffScreenIndicator->SetRenderTransformAngle(Degrees);
	}
}

void UBasicSingleIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	OffScreenIndicator = CreateWidgetInstance(*this, OffScreenIndicatorType, FName("OffScreenIndicator"));
	IndicatorContainer->AddChild(OffScreenIndicator);
	OnScreenIndicator = CreateWidgetInstance(*this, OnScreenIndicatorType, FName("OnScreenIndicator"));
	IndicatorContainer->AddChild(OnScreenIndicator);
}
