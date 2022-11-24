// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectScreen.h"

#include "CharacterSelect/CharacterSelectGameState.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelectScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	//PhoenixSelectModule->CharacterSelectDelegate.BindDynamic(this, &UCharacterSelectScreen::TrySelectPlayer);
	//CrowSelectModule->CharacterSelectDelegate.BindDynamic(this, &UCharacterSelectScreen::TrySelectPlayer);
	
	ReadyButton->OnClicked.AddDynamic(this, &UCharacterSelectScreen::TryReady);
	return true;
}

void UCharacterSelectScreen::TrySelectPlayer(EPlayerType PlayerSelected)
{
	PlayerSelectionDelegate.ExecuteIfBound(PlayerSelected);
}

//void UCharacterSelectScreen::PlayerSelectionUpdated(EPlayerType PlayerTypeSelected, APlayerState* PlayerThatSelected)
//{
	// if (PlayerTypeSelected == EPlayerType::CrowPlayer)
	// {
	// 	// player unselected crow
	// 	if (PlayerThatSelected == nullptr)
	// 	{
	// 		CrowSelectModule->NotifyPlayerUnSelectedModule();
	// 	}
	// 	// Player selected crow
	// 	else
	// 	{
	// 		CrowSelectModule->NotifyPlayerSelectedModule(PlayerThatSelected);
	// 	}
	// } else
	// {
	// 	// player unselected Phoenix
	// 	if (PlayerThatSelected == nullptr)
	// 	{
	// 		PhoenixSelectModule->NotifyPlayerUnSelectedModule();
	// 	}
	// 	// Player selected phoenix
	// 	else
	// 	{
	// 		PhoenixSelectModule->NotifyPlayerSelectedModule(PlayerThatSelected);
	// 	}
	// }
//}

void UCharacterSelectScreen::TryReady()
{
	PlayerReadyDelegate.ExecuteIfBound();
}
