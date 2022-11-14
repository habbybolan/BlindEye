// Copyright (C) Nicholas Johnson 2022


#include "HUD/Checklist.h"

#include "HUD/ChecklistItem.h"

void UChecklist::AddChecklistItem(uint8 ItemID, FString Text, uint8 AmountToComplete)
{
	UChecklistItem* ChecklistItem = Cast<UChecklistItem>(CreateWidgetInstance(*this, ChecklistItemType, TEXT("ChecklistItem")));
	//ChecklistItem->AddToPlayerScreen();
	ChecklistItem->SetInitialText(Text, AmountToComplete);
	//ChecklistContainer->AddChild(ChecklistItem);
	FChecklistItemData NewChecklistData;
	NewChecklistData.Initialize(ItemID, Text, AmountToComplete, ChecklistItem);
	ChecklistItems.Add(ItemID, NewChecklistData);

	ChecklistContainer->AddChild(ChecklistItem);
}

void UChecklist::UpdateChecklistItem(uint8 ID)
{
	if (ChecklistItems.Contains(ID))
	{
		ChecklistItems[ID].ChecklistItem->UpdateText();
	}
}

void UChecklist::RemoveFromParent()
{
	Super::RemoveFromParent();
	if (ChecklistContainer)
	{
		for (UWidget* Widget : ChecklistContainer->GetAllChildren())
		{
			Widget->RemoveFromParent();
		}
	}
}
