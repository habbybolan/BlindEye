// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectScreen.h"

#include "Gameplay/CharacterSelectGameState.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelectScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	//PhoenixSelectModule->CharacterSelectDelegate.BindDynamic(this, &UCharacterSelectScreen::TrySelectPlayer);
	//CrowSelectModule->CharacterSelectDelegate.BindDynamic(this, &UCharacterSelectScreen::TrySelectPlayer);
	return true;
}

void UCharacterSelectScreen::TrySelectPlayer(EPlayerType PlayerSelected)
{
	if (UWorld* World = GetWorld())
	{
		ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(UGameplayStatics::GetGameState(World));
		CharacterSelectGS->SER_PlayerTrySelect(PlayerSelected, GetOwningLocalPlayer());
	}
}

void UCharacterSelectScreen::PlayerSelectionUpdated(EPlayerType PlayerTypeSelected, ULocalPlayer* PlayerThatSelected)
{
	if (PlayerTypeSelected == EPlayerType::CrowPlayer)
	{
		CrowSelectModule->NotifyPlayerSelectedModule(PlayerThatSelected);
	} else
	{
		PhoenixSelectModule->NotifyPlayerSelectedModule(PlayerThatSelected);
	}
}
