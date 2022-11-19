// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "LobbyScreenBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "SessionRow.generated.h"

struct FServerData;
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

	DECLARE_DYNAMIC_DELEGATE_OneParam(FSessionRowSelectedSignature, USessionRow*, SessionRow);
	FSessionRowSelectedSignature SessionRowSelectedDelegate;

	void SetSelected(bool IsSelected);

	void InitializeData(FServerData Data, uint8 Index);

	uint8 RowIndex = 0;

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	FServerData SessionData;
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetSelected(bool IsSelected);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
