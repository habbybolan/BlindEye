// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectGameMode.h"

#include "CharacterSelect/CharacterSelectPlayerController.h"
#include "Gameplay/BlindEyeGameInstance.h"

void ACharacterSelectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACharacterSelectPlayerController* PlayerController = Cast<ACharacterSelectPlayerController>(NewPlayer);
	PlayerController->CLI_LoadCharacterSelectScreen();
}
