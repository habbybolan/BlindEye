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
	CurrCount = 0;
	CachedText = text;
	FString newText = CreateText();
	Text->SetText(FText::FromString(newText));
}

void UChecklistItem::UpdateText()
{
	CurrCount++;
	FString oldText = Text->GetText().ToString();
	FString newText = CreateText();
	Text->SetText(FText::FromString(newText));

	if (CurrCount >= MaxCount)
	{
		Text->SetRenderOpacity(0.4);
		CompletedBox->SetCheckedState(ECheckBoxState::Checked);
	}
} 
 
FString UChecklistItem::CreateText()
{ 
	FString newText = CachedText + " " +  FString::FromInt(CurrCount) + "/" + FString::FromInt(MaxCount);
	return newText;
}