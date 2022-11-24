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
	SetAsLANCheckbox->OnCheckStateChanged.AddDynamic(this, &UCreateLobbyScreen::OnLANCheckboxSelected);
	return true;
}

void UCreateLobbyScreen::OnNewSessionPressed()
{
	if (SessionMenuInterface == nullptr) return;
	FString LobbyName = LobbyNameEditText->GetText().IsEmpty() ? "BlindEyeServer" : LobbyNameEditText->GetText().ToString();
	SessionMenuInterface->Host(LobbyName);
}

void UCreateLobbyScreen::OnLANCheckboxSelected(bool IsChecked)
{
	if (SessionMenuInterface == nullptr) return;
	SessionMenuInterface->SetIsLAN(IsChecked);
}
