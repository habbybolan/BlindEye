// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectGameMode.h"

#include "CharacterSelect/CharacterSelectGameState.h"
#include "CharacterSelect/CharacterSelectPlayerController.h"
#include "Gameplay/BlindEyeGameInstance.h"

void ACharacterSelectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACharacterSelectPlayerController* PlayerController = Cast<ACharacterSelectPlayerController>(NewPlayer);
	PlayerController->CLI_LoadCharacterSelectScreen();

	ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(GameState);
	CharacterSelectGS->OnPlayerChanged(true, NewPlayer);
}

void ACharacterSelectGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(GameState);
	CharacterSelectGS->OnPlayerChanged(false, Exiting);
}
