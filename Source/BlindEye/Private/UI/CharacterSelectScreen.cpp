// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectScreen.h"

#include "CharacterSelect/CharacterSelectGameState.h"
#include "Interfaces/SessionMenuInterface.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelectScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	ReadyButton->OnClicked.AddDynamic(this, &UCharacterSelectScreen::TryReady);
	LeaveLobbyButton->OnClicked.AddDynamic(this, &UCharacterSelectScreen::LeaveCharacterSelect);
	return true;
}

void UCharacterSelectScreen::SetSessionMenuInterface(TScriptInterface<ISessionMenuInterface> sessionMenuInterface)
{
	SessionMenuInterface = sessionMenuInterface;
	LobbyNameText->SetText(FText::FromString(SessionMenuInterface->GetLobbyName()));
}

void UCharacterSelectScreen::FadeIntoBlack(float Duration)
{
	BP_FadeIntoBlack(Duration);
}

void UCharacterSelectScreen::TryReady()
{
	ACharacterSelectPlayerController* PlayerController = Cast<ACharacterSelectPlayerController>(GetOwningPlayer());
	PlayerController->SER_SetPlayerReadied();
}

void UCharacterSelectScreen::LeaveCharacterSelect()
{
	SessionMenuInterface->EndSession();
}

void UCharacterSelectScreen::NotifyPlayersInSessionUpdated(uint8 NumPlayers)
{
	FString Text = "In Lobby " + FString::FromInt(NumPlayers) + "/2";
	NumPlayersInLobbyText->SetText(FText::FromString(Text));
}
