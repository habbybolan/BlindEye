// Copyright (C) Nicholas Johnson 2022


#include "HUD/Checklist.h"

#include "HUD/ChecklistItem.h"

void UChecklist::AddChecklistItem(uint8 ItemID, FString Text, uint8 AmountToComplete)
{
	if (ChecklistItems.Contains(ItemID)) return;

	FString name =  TEXT("ChecklistItem") + FString::FromInt(ItemID);
	UChecklistItem* ChecklistItem = Cast<UChecklistItem>(CreateWidgetInstance(*this, ChecklistItemType, FName(*name)));
	//ChecklistItem->AddToPlayerScreen();
	ChecklistItem->SetInitialText(Text, AmountToComplete);
	//ChecklistContainer->AddChild(ChecklistItem);
	FChecklistItemData NewChecklistData;
	NewChecklistData.Initialize(ItemID, Text, AmountToComplete, ChecklistItem);
	ChecklistItems.Add(ItemID, NewChecklistData);

	ChecklistContainer->AddChildToVerticalBox(ChecklistItem);
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
