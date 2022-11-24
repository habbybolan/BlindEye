// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectPlayerController.h"

#include "CharacterSelect/CharacterSelectGameState.h"
#include "Gameplay/BlindEyeGameInstance.h"
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

void ACharacterSelectPlayerController::SER_SetPlayerReadied_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(UGameplayStatics::GetGameState(World));
		CharacterSelectGS->SER_PlayerTryReady(this);
	}
}

void ACharacterSelectPlayerController::CLI_LoadCharacterSelectScreen_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(UGameplayStatics::GetGameInstance(World));
		BlindEyeGI->LoadCharacterSelect();
	}
}
