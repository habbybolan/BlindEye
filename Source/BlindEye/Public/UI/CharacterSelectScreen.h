// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "CharacterSelectModule.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectScreen.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UCharacterSelectScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCharacterSelectModule* PhoenixSelectModule;
 
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCharacterSelectModule* CrowSelectModule;

	virtual bool Initialize() override;

	void PlayerSelectionUpdated(EPlayerType PlayerTypeSelected, ULocalPlayer* PlayerThatSelected);

protected:

	UFUNCTION()
	void TrySelectPlayer(EPlayerType PlayerSelected);
};
