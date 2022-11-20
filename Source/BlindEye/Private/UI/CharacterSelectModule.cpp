// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectModule.h"

bool UCharacterSelectModule::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	ReadyButton->OnClicked.AddDynamic(this, &UCharacterSelectModule::OnReadyButtonSelected);
	SelectCharacterButton->OnClicked.AddDynamic(this, &UCharacterSelectModule::OnCharacterSelected);
	
	BaseAccentColor = ReadyButton->BackgroundColor;
	return true;
}

void UCharacterSelectModule::NotifyPlayerSelectedModule(ULocalPlayer* LocalPlayer)
{
	PlayerThatSelected = LocalPlayer;
	PlayerNameText->Text = FText::FromString(LocalPlayer->GetName());
	
	if (LocalPlayer == GetOwningLocalPlayer())
	{
		BP_OwningPlayerSelected();
		SelectCharacterButton->SetBackgroundColor(FLinearColor(AccentColor));
	} else
	{
		BP_OtherPlayerSelected();
		SelectCharacterButton->SetBackgroundColor(FLinearColor(AccentColor));
	}
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
