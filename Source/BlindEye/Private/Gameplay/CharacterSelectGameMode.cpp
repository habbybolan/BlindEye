// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/CharacterSelectGameMode.h"

#include "Characters/CharacterSelectPlayerController.h"

void ACharacterSelectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "INITIALIZE UI GM");
	ACharacterSelectPlayerController* CharacterSelectPC = Cast<ACharacterSelectPlayerController>(NewPlayer);
	CharacterSelectPC->CLI_InitializeUI();
}
