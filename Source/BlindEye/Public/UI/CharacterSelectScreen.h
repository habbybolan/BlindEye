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

	// UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	// UCharacterSelectModule* PhoenixSelectModule;
 
	// UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	// UCharacterSelectModule* CrowSelectModule;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* ReadyButton;

	virtual bool Initialize() override;
	
	DECLARE_DYNAMIC_DELEGATE(FPlayerReadySignature);
	FPlayerReadySignature PlayerReadyDelegate;

protected:

	UFUNCTION() 
	void TryReady();
};
