// Copyright (C) Nicholas Johnson 2022


#include "HUD/PlayerScreenIndicator.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UPlayerScreenIndicator::SetTarget(TScriptInterface<IIndicatorInterface> target)
{
	Super::SetTarget(target);
	bInitialized = true;
}

void UPlayerScreenIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// prevent calculations if not initialized with player target
	if (!bInitialized) return;
	
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
			//CurrIndicator = PlayerDownedOnScreenWidget;
		} else
		{
			//CurrIndicator = PlayerDownedOffScreenWidget;
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
		CurrIndicator->SetPositionInViewport(ScreenPosition);
		
		// UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(CurrIndicator);
		// if (CanvasSlot)
		// {
		// 	FAnchors Anchors;
		// 	Anchors.Minimum = FVector2D(ScreenPosition.X, ScreenPosition.Y);
		// 	Anchors.Maximum = FVector2D(ScreenPosition.X, ScreenPosition.Y);
		// 	CanvasSlot->SetAnchors(Anchors);
		// }
	}
}

void UPlayerScreenIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerMarkedOnScreenWidget = CreateWidgetInstance(*this, PlayerMarkedOnScreenWidgetType, FName("MarkedOnScreen"));
	// PlayerMarkedOnScreenWidget->AddToViewport();
	// PlayerMarkedOffScreenWidget = CreateWidgetInstance(*this, PlayerMarkedOffScreenWidgetType, FName("MarkedOffScreen"));
	// PlayerMarkedOffScreenWidget->AddToViewport();
	PlayerDownedOnScreenWidget = CreateWidgetInstance(*this, PlayerDownedOnScreenWidgetType, FName("DownedOnScreen"));
	PlayerDownedOnScreenWidget->AddToViewport();
	PlayerDownedOffScreenWidget = CreateWidgetInstance(*this, PlayerDownedOffScreenWidgetType, FName("DownedOffScreen"));
	PlayerDownedOffScreenWidget->AddToViewport();
	PlayerNormalOffScreenWidget = CreateWidgetInstance(*this, PlayerNormalOffScreenWidgetType, FName("NormalOffScreen"));
	PlayerNormalOffScreenWidget->AddToViewport();
	HideAllWidgets();
} 


void UPlayerScreenIndicator::HideAllWidgets()
{
	// PlayerMarkedOnScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	// PlayerMarkedOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerDownedOnScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerDownedOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerNormalOffScreenWidget->SetVisibility(ESlateVisibility::Hidden);
}
