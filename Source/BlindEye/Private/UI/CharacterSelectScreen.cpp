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
	
	ReadyButton->OnClicked.AddDynamic(this, &UCharacterSelectScreen::TryReady);
	return true;
}

void UCharacterSelectScreen::TryReady()
{
	PlayerReadyDelegate.ExecuteIfBound();
}
