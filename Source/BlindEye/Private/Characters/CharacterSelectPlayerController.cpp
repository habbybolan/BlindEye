// Copyright (C) Nicholas Johnson 2022


#include "Characters/CharacterSelectPlayerController.h"

#include "Gameplay/CharacterSelectGameState.h"
#include "Kismet/GameplayStatics.h"

void ACharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterSelectPlayerController::UpdatePlayerSelectedCharacter(EPlayerType PlayerTypeSelected,
	APlayerState* PlayerThatSelected)
{
	CharacterSelectScreen->PlayerSelectionUpdated(PlayerTypeSelected, PlayerThatSelected);
}

void ACharacterSelectPlayerController::UpdateReadyState(bool IsReady)
{
	// TODO:
}

void ACharacterSelectPlayerController::PlayerSelection(EPlayerType CharacterSelected)
{
	SER_PlayerSelection(CharacterSelected);
}

void ACharacterSelectPlayerController::SER_PlayerSelection_Implementation(EPlayerType CharacterSelected)
{
	if (UWorld* World = GetWorld())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "[CharacterSelectPlayerController::PlayerSelection] Make selection");
		ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(UGameplayStatics::GetGameState(World));
		CharacterSelectGS->PlayerTrySelect(CharacterSelected, this);
	}
}

void ACharacterSelectPlayerController::SER_ReadyUp_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "[CharacterSelectPlayerController::PlayerSelection] Make selection");
		ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(UGameplayStatics::GetGameState(World));
		CharacterSelectGS->PlayerTryReady(this);
	}
}

void ACharacterSelectPlayerController::CLI_InitializeUI_Implementation()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "INITIALIZE UI");
	CharacterSelectScreen = Cast<UCharacterSelectScreen>(CreateWidget(this, CharacterSelectScreenType));
	CharacterSelectScreen->AddToViewport();

	CharacterSelectScreen->PlayerSelectionDelegate.BindDynamic(this, &ACharacterSelectPlayerController::PlayerSelection);
	CharacterSelectScreen->PlayerReadyDelegate.BindDynamic(this, &ACharacterSelectPlayerController::SER_ReadyUp);
}
