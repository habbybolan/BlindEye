// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectWidget.generated.h"

struct FCharacterSelectedInfo;
class UTextBlock;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerNameText;

	void UpdateInfo(FCharacterSelectedInfo CharacterSelectInfo);
	
};
