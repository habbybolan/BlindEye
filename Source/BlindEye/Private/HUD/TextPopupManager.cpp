// Copyright (C) Nicholas Johnson 2022

#include "TextPopupManager.h"
#include "Components/VerticalBox.h"
#include "HUD/TextPopupWidget.h"

void UTextPopupManager::NativeConstruct()
{
	Super::NativeConstruct();
}
 
void UTextPopupManager::AddTextPopup(const FString& Text, ETextPopupType TextPopupType, float Duration)
{
	FString PopupName = TEXT("TextPopupWidget") + FString::FromInt(WidgetCount++);
	UTextPopupWidget* TextPopup = Cast<UTextPopupWidget>(CreateWidgetInstance(*this, PopupWidgetType, FName(*PopupName)));
	if (TextPopup)
	{
		TextPopup->InitializeTextPopup(Text, TextPopupType, Duration);
		TextPopupContainer->AddChildToVerticalBox(TextPopup);
		AddedTextSnippets.Add(TextPopup);
		TextPopup->PopupToRemoveDelegate.BindDynamic(this, &UTextPopupManager::PopupToRemove);
	}
}

void UTextPopupManager::PopupToRemove(UTextPopupWidget* PopupToRemove)
{
	const uint32 Index = AddedTextSnippets.IndexOfByPredicate([PopupToRemove](const UTextPopupWidget* PopupWidget)
	{
		return PopupToRemove == PopupWidget;
	}); 
	// remove reference to widget and remove from viewport
	if (Index != INDEX_NONE)
	{
		AddedTextSnippets.RemoveAtSwap(Index, 1, false);
		PopupToRemove->RemoveFromParent();
	}
}
