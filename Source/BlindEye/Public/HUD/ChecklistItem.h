// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChecklistItem.generated.h"

class UCheckBox;
class UTextBlock;


/**
 * 
 */
UCLASS()
class BLINDEYE_API UChecklistItem : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void SetInitialText(FString& text, uint8 maxCount);

	// Update text by incrementing counter, finishing task if counter passing max count
	void UpdateText();

protected:

	uint8 MaxCount;
	uint8 CurrCount = 0;
	FString CachedText;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UCheckBox* CompletedBox;
 
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Text;

	FString CreateText();
};
