// Copyright (C) Nicholas Johnson 2022


#include "HUD/TextPopupWidget.h"

void UTextPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Durations.SetNum((uint8)ETextPopupType::Count);
}

void UTextPopupWidget::InitializeTextPopup(FString Text, ETextPopupType PopupType, float Duration)
{
	UWorld* World = GetWorld();
	if (World)
	{
		BP_AddTextPopup_CLI(PopupType, Text);
		// Use passed in duration if not 0, otherwise use default based on PopupType
		float ActualDuration = Duration != 0 ? Duration : Durations[(uint8)PopupType]; 
		World->GetTimerManager().SetTimer(DurationTimerHandle, this, &UTextPopupWidget::RemoveTextPopup, ActualDuration, false);
	}
}

void UTextPopupWidget::RemoveTextPopup()
{
	PopupToRemoveDelegate.ExecuteIfBound(this);
}
