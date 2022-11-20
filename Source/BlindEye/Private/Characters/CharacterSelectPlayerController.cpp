// Copyright (C) Nicholas Johnson 2022


#include "Characters/CharacterSelectPlayerController.h"

void ACharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CharacterSelectScreen = Cast<UCharacterSelectScreen>(CreateWidget(this, CharacterSelectScreenType));
	CharacterSelectScreen->AddToViewport();

	SetShowMouseCursor(true);
}

void ACharacterSelectPlayerController::UpdatePlayerSelectedCharacter(EPlayerType PlayerTypeSelected,
	ULocalPlayer* PlayerThatSelected)
{
	CharacterSelectScreen->PlayerSelectionUpdated(PlayerTypeSelected, PlayerThatSelected);
}

void ACharacterSelectPlayerController::UpdateReadyState(bool IsReady)
{
	// TODO:
}
