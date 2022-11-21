// Copyright (C) Nicholas Johnson 2022


#include "UI/CreateLobbyScreen.h"

#include "Components/Button.h"
#include "Gameplay/BlindEyeGameInstance.h"
#include "Interfaces/SessionMenuInterface.h"

bool UCreateLobbyScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	NewSessionButton->OnClicked.AddDynamic(this, &UCreateLobbyScreen::OnNewSessionPressed);
	LANButton->OnClicked.AddDynamic(this, &UCreateLobbyScreen::OnLANPressed);
	return true;
}

void UCreateLobbyScreen::OnNewSessionPressed()
{
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->SetGameAsLan(false);
	
	if (SessionMenuInterface == nullptr) return;
	SessionMenuInterface->Host("BlindEyeServer");
}

void UCreateLobbyScreen::OnLANPressed()
{
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->SetGameAsLan(true);
}
