// Copyright (C) Nicholas Johnson 2022


#include "HUD/PlayerScreenIndicator.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Engine.h"
#include "HUD/ScreenIndicatorCustomRotationIcon.h"

void UPlayerScreenIndicator::SetTarget(UObject* target)
{
	Super::SetTarget(target);
	bInitialized = true;
}

void UPlayerScreenIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// prevent calculations if not initialized with player target
	if (!bInitialized)
	{
		return;
	}
	
	FVector2D ScreenPosition;
	float Degrees;
	bool IsOnScreen;

	FindScreenEdgeLocationForWorldLocation(ScreenPosition, Degrees, IsOnScreen);
	HideAllWidgets();
 
	UUserWidget* CurrIndicator = nullptr;
	if (bisPlayerDowned)
	{
		if (IsOnScreen)
		{
			CurrIndicator = PlayerDownedOnScreenWidget;
		} else
		{
			CurrIndicator = PlayerDownedOffScreenWidget;
		}
	}
	else if (bIsPlayerMarked)
	{
		if (IsOnScreen)
		{
			if (PlayerMarkedOnScreenWidget != nullptr)
			{
				CurrIndicator = PlayerDownedOnScreenWidget;
			}
		} else
		{
			CurrIndicator = PlayerMarkedOffScreenWidget;
		}	
	}
	else
	{
		if (!IsOnScreen)
		{
			CurrIndicator = PlayerNormalOffScreenWidget;
		} 
	}

	// Show and Set position of widget on screen
	if (CurrIndicator != nullptr)
	{
		CurrIndicator->SetVisibility(ESlateVisibility::Visible);
		UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(CurrIndicator);
		CanvasSlot->SetPosition(ScreenPosition - FVector2D(1, 0) * CanvasSlot->GetSize().X / 2);

		if (!IsOnScreen)
		{
			// If it has custom rotation, then use custom rotation
			if (UScreenIndicatorCustomRotationIcon* Icon = Cast<UScreenIndicatorCustomRotationIcon>(CurrIndicator))
			{
				Icon->ApplyRotation(Degrees);
			} else
			{
				CurrIndicator->SetRenderTransformAngle(Degrees);
			}
		}
	}
}

void UPlayerScreenIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PlayerMarkedOnScreenWidgetType))
	{
		PlayerMarkedOnScreenWidget = CreateWidgetInstance(*this, PlayerMarkedOnScreenWidgetType, FName("MarkedOnScreen"));
		IndicatorContainer->AddChild(PlayerMarkedOnScreenWidget);
	} else PlayerMarkedOnScreenWidget = nullptr;
	
	PlayerMarkedOffScreenWidget = CreateWidgetInstance(*this, PlayerMarkedOffScreenWidgetType, FName("MarkedOffScreen"));
	IndicatorContainer->AddChild(PlayerMarkedOffScreenWidget);
	PlayerDownedOnScreenWidget = CreateWidgetInstance(*this, PlayerDownedOnScreenWidgetType, FName("DownedOnScreen"));
	IndicatorContainer->AddChild(PlayerDownedOnScreenWidget);
	PlayerDownedOffScreenWidget = CreateWidgetInstance(*this, PlayerDownedOffScreenWidgetType, FName("DownedOffScreen"));
	IndicatorContainer->AddChild(PlayerDownedOffScreenWidget);
	PlayerNormalOffScreenWidget = CreateWidgetInstance(*this, PlayerNormalOffScreenWidgetType, FName("NormalOffScreen"));
	IndicatorContainer->AddChild(PlayerNormalOffScreenWidget);
	HideAllWidgets();
} 


void UPlayerScreenIndicator::HideAllWidgets()
{
	if (PlayerMarkedOnScreenWidget != nullptr)
	{
		PlayerMarkedOnScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	PlayerMarkedOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerDownedOnScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerDownedOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerNormalOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
}
