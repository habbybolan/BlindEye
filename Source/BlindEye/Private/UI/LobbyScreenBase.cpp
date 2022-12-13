// Copyright (C) Nicholas Johnson 2022


#include "UI/LobbyScreenBase.h"

#include "Characters/BlindEyePlayerController.h"
#include "UI/SessionRow.h"

void ULobbyScreenBase::NativeConstruct()
{
	Super::NativeConstruct();
}

bool ULobbyScreenBase::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	
	return true;
}

void ULobbyScreenBase::InitializeSessionList(TArray<FServerData> ServerDataList)
{
	// Override if child loads session list
}

void ULobbyScreenBase::TearDown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	ABlindEyePlayerController* PlayerController = Cast<ABlindEyePlayerController>(World->GetFirstPlayerController());
	if (PlayerController == nullptr) return;

	// Set the Input Mode for game mode: allows only the player input / player controller to respond to user input.
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	//PlayerController->UnLockInput();
	PlayerController->bShowMouseCursor = false;
}
