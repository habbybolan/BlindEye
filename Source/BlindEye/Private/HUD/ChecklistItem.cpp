// Copyright (C) Nicholas Johnson 2022


#include "HUD/ChecklistItem.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void UChecklistItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UChecklistItem::SetInitialText(FString& text, uint8 maxCount)
{
	MaxCount = maxCount;
	FString newText = CreateText(text, 0);
	Text->SetText(FText::FromString(newText));
}

void UChecklistItem::UpdateText(uint8 newCount)
{ 
	FString oldText = Text->GetText().ToString();
	FString newText = CreateText(oldText, newCount);
	Text->SetText(FText::FromString(newText));

	if (newCount >= MaxCount)
	{
		Text->SetRenderOpacity(0.4);
		CompletedBox->SetCheckedState(ECheckBoxState::Checked);
	}
} 
 
FString UChecklistItem::CreateText(FString& text, uint8 newCount)
{ 
	FString newText = text + " " +  FString::FromInt(newCount) + FString::FromInt(MaxCount);
	return newText;
}