// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectModule.h"

#include "GameFramework/PlayerState.h"

bool UCharacterSelectModule::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	SelectCharacterButton->OnClicked.AddDynamic(this, &UCharacterSelectModule::OnCharacterSelected);
	BaseAccentColor = FColor::White;
	return true;
}

void UCharacterSelectModule::NotifyPlayerSelectedModule(APlayerState* LocalPlayer)
{
	PlayerThatSelected = LocalPlayer;
	
	if (LocalPlayer == GetOwningPlayerState())
	{
		BP_OwningPlayerSelected();
	} else
	{
		BP_OtherPlayerSelected();
	}
	SelectCharacterButton->SetBackgroundColor(FLinearColor(AccentColor));
	PlayerNameText->Text = FText::FromString(LocalPlayer->GetName());
}

void UCharacterSelectModule::NotifyPlayerUnSelectedModule()
{
	PlayerThatSelected = nullptr;
	SelectCharacterButton->SetBackgroundColor(FLinearColor(BaseAccentColor));
	BP_PlayerUnSelected();
}

void UCharacterSelectModule::OnReadyButtonSelected()
{
	PlayerReadyDelegate.ExecuteIfBound(PlayerType);
}

void UCharacterSelectModule::OnCharacterSelected()
{
	// TODO: wait for NotifyPlayerSelectedModule call to actually select, notify character select occurred.
	CharacterSelectDelegate.ExecuteIfBound(PlayerType);
}
