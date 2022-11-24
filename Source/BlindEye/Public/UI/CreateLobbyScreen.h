// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/CheckBox.h"
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

	virtual bool Initialize() override;

protected:

	UFUNCTION()
	void OnNewSessionPressed();
 
	UFUNCTION() 
	void OnLANCheckboxSelected(bool IsChecked);
	
};
