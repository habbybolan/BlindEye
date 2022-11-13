// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextPopupWidget.generated.h"

UENUM(BlueprintType)
enum class ETextPopupType : uint8
{
	Info,
	UrgentInfo,
	Warning,
 
	Count UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API UTextPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FPopupToRemoveSignature, UTextPopupWidget*, WidgetToRemove);
	FPopupToRemoveSignature PopupToRemoveDelegate;

	// Duration for Info, UrgentInfo, and Warning
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Info, UrgentInfo, and Warning default durations"))
	TArray<float> Durations;

	void InitializeTextPopup(FString Text, ETextPopupType PopupType, float Duration);

protected:

	FTimerHandle DurationTimerHandle;
	UFUNCTION()
	void RemoveTextPopup();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AddTextPopup_CLI(ETextPopupType PopupType, const FString& Text);
	
};
