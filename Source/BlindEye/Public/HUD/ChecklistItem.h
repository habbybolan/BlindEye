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
	void UpdateText(uint8 newCount);

protected:

	uint8 MaxCount;
 
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UCheckBox* CompletedBox;
 
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Text;

	FString CreateText(FString& text, uint8 newCount);
};
