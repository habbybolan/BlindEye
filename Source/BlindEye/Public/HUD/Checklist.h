// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Checklist.generated.h"

USTRUCT()
struct FChecklistItemData
{
	GENERATED_BODY()

	void Initialize(uint8 id, FString text, uint8 amountToComplete)
	{
		ID = id;
		Text = text;
		AmountToComplete = amountToComplete;
	}

	uint8 ID;
	FString Text;
	uint8 AmountToComplete;
};
/**
 * 
 */
UCLASS()
class BLINDEYE_API UChecklist : public UUserWidget
{
	GENERATED_BODY()

public:

	void AddChecklistItem(uint8 ItemID, FString Text, uint8 AmountToComplete = 1);

	void UpdateChecklistItem(uint8 ID);

protected:
	
	TMap<uint8, FChecklistItemData> ChecklistItems;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UVerticalBox* ChecklistContainer;
	
};
