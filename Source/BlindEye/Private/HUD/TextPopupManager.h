// Copyright (C) Nicholas Johnson 2022
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextPopupManager.generated.h"

enum class ETextPopupType : uint8;
class UTextPopupWidget;
class UVerticalBox;

/**
 * Generalized way of adding temporary text to the screen
 */
UCLASS(Blueprintable) 
class BLINDEYE_API UTextPopupManager : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<UTextPopupWidget> PopupWidgetType;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UVerticalBox* TextPopupContainer;

	UFUNCTION(BlueprintCallable)
	void AddTextPopup(const FString& Text, ETextPopupType TextPopupType, float Duration = 0);

protected:

	UPROPERTY()
	TArray<UTextPopupWidget*> AddedTextSnippets;
 
	UFUNCTION()
	void PopupToRemove(UTextPopupWidget* PopupToRemove);

	uint8 WidgetCount = 0;
};
