// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "LoadingIcon.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "UI/LobbyScreenBase.h"
#include "CreateLobbyScreen.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UCreateLobbyScreen : public ULobbyScreenBase
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UButton* NewSessionButton;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SetAsLANCheckbox;

	UPROPERTY(meta = (BindWidget))
	UEditableText* LobbyNameEditText;

	UPROPERTY(meta = (BindWidget))
	ULoadingIcon* LoadingIcon;
	
	virtual bool Initialize() override;

	virtual void LoadingStarted() override;
	virtual void LoadingFailed(FString Message) override;
	virtual void LoadingSucceeded() override;

protected:

	UFUNCTION()
	void OnNewSessionPressed();
 
	UFUNCTION() 
	void OnLANCheckboxSelected(bool IsChecked);
	
};
