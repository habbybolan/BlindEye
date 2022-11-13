// Copyright (C) Nicholas Johnson 2022
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextPopupManager.generated.h"

enum class ETextPopupType : uint8;
class UTextPopupWidget;

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

	void AddTextPopup(FString Text, ETextPopupType TextPopupType, float Duration = 0);

protected:

	UPROPERTY()
	TArray<UTextPopupWidget*> AddedTextSnippets;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AddTextPopup_CLI(UTextPopupWidget* TextPopup);
 
	UFUNCTION()
	void PopupToRemove(UTextPopupWidget* PopupToRemove);
};
