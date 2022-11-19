// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/CharacterSelectScreen.h"
#include "CharacterSelectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACharacterSelectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCharacterSelectScreen> CharacterSelectScreenType;
 
	void UpdatePlayerSelectedCharacter(EPlayerType PlayerTypeSelected, ULocalPlayer* PlayerThatSelected);
	void UpdateReadyState(bool IsReady);

protected:

	UPROPERTY()
	UCharacterSelectScreen* CharacterSelectScreen;
	
};
