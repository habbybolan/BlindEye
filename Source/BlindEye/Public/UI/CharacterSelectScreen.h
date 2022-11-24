// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "CharacterSelectModule.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectScreen.generated.h"

class ISessionMenuInterface;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UCharacterSelectScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* ReadyButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LobbyNameText;

	UPROPERTY(meta=(BindWidget))
	UButton* LeaveLobbyButton;

	virtual bool Initialize() override;

	void SetSessionMenuInterface(TScriptInterface<ISessionMenuInterface> SessionMenuInterface);

	

protected:

	UFUNCTION() 
	void TryReady();

	UFUNCTION()
	void LeaveCharacterSelect();

	UPROPERTY()
	TScriptInterface<ISessionMenuInterface> SessionMenuInterface;
};
