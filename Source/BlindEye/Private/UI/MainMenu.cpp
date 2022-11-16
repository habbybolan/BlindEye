// Copyright (C) Nicholas Johnson 2022


#include "UI/MainMenu.h"

#include "Components/Button.h"
#include "Interfaces/SessionMenuInterface.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	NewSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnNewSessionPressed);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinSelectedSession);
}

void UMainMenu::InitializeSessionList(TArray<FServerData> ServerData)
{
	// TODO:
}

void UMainMenu::OnNewSessionPressed()
{
	if (SessionMenuInterface == nullptr) return;
	SessionMenuInterface->Host("BlindEyeServer");
}

void UMainMenu::OnJoinSelectedSession()
{
	// Calling the method Join Session with a specific index
	// SelectedScrollIndex is an uint32 representing the row on a scroll // rect
	SessionMenuInterface->JoinSession(SelectedScrollIndex);
}
