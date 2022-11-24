// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectPlayerController.h"

#include "CharacterSelect/CharacterSelectGameState.h"
#include "Kismet/GameplayStatics.h"

void ACharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
}

void ACharacterSelectPlayerController::UpdateReadyState(bool IsReady)
{
	// TODO:
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

	CharacterSelectScreen->PlayerReadyDelegate.BindDynamic(this, &ACharacterSelectPlayerController::SER_ReadyUp);
}
