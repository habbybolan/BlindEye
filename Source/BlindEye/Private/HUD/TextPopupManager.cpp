// Copyright (C) Nicholas Johnson 2022

#include "TextPopupManager.h"

#include "HUD/TextPopup.h"
#include "HUD/TextPopupWidget.h"

void UTextPopupManager::NativeConstruct()
{
	Super::NativeConstruct();
}
 
void UTextPopupManager::AddTextPopup(FString Text, ETextPopupType TextPopupType, float Duration)
{
	UTextPopupWidget* TextPopup = Cast<UTextPopupWidget>(CreateWidgetInstance(*this, PopupWidgetType, FName("TextPopupWidget")));
	if (TextPopup)
	{
		TextPopup->InitializeTextPopup(Text, TextPopupType, Duration);
		// let BP add to viewport in proper container
		BP_AddTextPopup_CLI(TextPopup);
		AddedTextSnippets.Add(TextPopup);
		TextPopup->PopupToRemoveDelegate.BindDynamic(this, &UTextPopupManager::PopupToRemove);
	}
}

void UTextPopupManager::PopupToRemove(UTextPopupWidget* PopupToRemove)
{
	// remove reference to widget and remove from viewport
	// if (const uint32 Index = AddedTextSnippets.IndexOfByPredicate(PopupToRemove) != INDEX_NONE)
	// {
	// 	AddedTextSnippets.RemoveAtSwap(Index, 1, false);
	// 	PopupToRemove->RemoveFromParent();
	// }
}
