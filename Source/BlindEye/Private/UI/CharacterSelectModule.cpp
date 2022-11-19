// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectModule.h"

bool UCharacterSelectModule::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	//ReadyButton->OnClicked.AddDynamic(this, &UCharacterSelectModule::OnReadyButtonSelected);
	return true;
}

void UCharacterSelectModule::NotifyPlayerSelectedModule(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer == GetOwningLocalPlayer())
	{
		// TODO: Show different visuals if you selected the character
	} else
	{
		// TODO: Otherwise other player selected character, show their username om module
	}
}

void UCharacterSelectModule::OnReadyButtonSelected()
{
}

FReply UCharacterSelectModule::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CharacterSelectDelegate.ExecuteIfBound(PlayerType);
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
