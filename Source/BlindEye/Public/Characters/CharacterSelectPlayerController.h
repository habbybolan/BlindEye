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
 
	void UpdatePlayerSelectedCharacter(EPlayerType PlayerTypeSelected, APlayerState* PlayerThatSelected);
	void UpdateReadyState(bool IsReady);

	UFUNCTION(Client, Reliable)
	void CLI_InitializeUI();

protected:

	UPROPERTY()
	UCharacterSelectScreen* CharacterSelectScreen;

	UFUNCTION()
	void PlayerSelection(EPlayerType CharacterSelected);
	UFUNCTION(Server, Reliable)
	void SER_ReadyUp();
	UFUNCTION(Server, Reliable) 
	void SER_PlayerSelection(EPlayerType CharacterSelected);
};
