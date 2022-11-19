// Copyright (C) Nicholas Johnson 2022


#include "UI/CreateLobbyScreen.h"

#include "Components/Button.h"
#include "Interfaces/SessionMenuInterface.h"

bool UCreateLobbyScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	NewSessionButton->OnClicked.AddDynamic(this, &UCreateLobbyScreen::OnNewSessionPressed);
	return true;
}

void UCreateLobbyScreen::OnNewSessionPressed()
{
	if (SessionMenuInterface == nullptr) return;
	SessionMenuInterface->Host("BlindEyeServer");
}
