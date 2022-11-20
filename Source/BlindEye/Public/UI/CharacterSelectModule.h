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
	UButton* SelectCharacterButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(EditDefaultsOnly)
	FColor AccentColor;

	UPROPERTY(EditDefaultsOnly)
	EPlayerType PlayerType;
 
	DECLARE_DYNAMIC_DELEGATE_OneParam(FCharacterSelectedSignature, EPlayerType, SelectedPlayerType);
	FCharacterSelectedSignature CharacterSelectDelegate;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerReadySiganture, EPlayerType, SelectedPlayerType);
	FPlayerReadySiganture PlayerReadyDelegate;
	
	void NotifyPlayerSelectedModule(APlayerState* LocalPlayer);
	void NotifyPlayerUnSelectedModule();

protected:

	FLinearColor BaseAccentColor;

	UPROPERTY()
	APlayerState* PlayerThatSelected = nullptr;
	
	UFUNCTION()
	void OnReadyButtonSelected();

	UFUNCTION()
	void OnCharacterSelected();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OtherPlayerSelected();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OwningPlayerSelected();
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayerUnSelected();
	
};
