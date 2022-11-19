// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "SessionRow.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API USessionRow : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UTextBlock* ServerName;

	void SetSelected(bool IsSelected);

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetSelected(bool IsSelected);
	
};
