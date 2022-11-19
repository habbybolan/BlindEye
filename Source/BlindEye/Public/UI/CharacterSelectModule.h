// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CharacterSelectModule.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UCharacterSelectModule : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ReadyButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(EditDefaultsOnly)
	EPlayerType PlayerType;
 
	DECLARE_DYNAMIC_DELEGATE_OneParam(FCharacterSelectedSignature, EPlayerType, SelectedPlayerType);
	FCharacterSelectedSignature CharacterSelectDelegate;

	void NotifyPlayerSelectedModule(ULocalPlayer* LocalPlayer);

	UFUNCTION()
	void OnReadyButtonSelected();

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
