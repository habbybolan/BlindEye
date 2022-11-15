// Copyright (C) Nicholas Johnson 2022


#include "HUD/ChecklistItem.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

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
	CompletedBox->SetCheckedState(ECheckBoxState::Unchecked);
}

void UChecklistItem::UpdateText()
{
	// Prevent going over max count
	CurrCount = UKismetMathLibrary::Min(CurrCount + 1, MaxCount);
	
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
	// If less than one max count, then dont display amount
	if (MaxCount <= 0) return CachedText;
	FString newText = CachedText + " " +  FString::FromInt(CurrCount) + "/" + FString::FromInt(MaxCount);
	return newText;
}