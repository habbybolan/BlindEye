// Copyright (C) Nicholas Johnson 2022


#include "UI/CharacterSelectWidget.h"

#include "CharacterSelect/CharacterSelectModel.h"

bool UCharacterSelectWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;
	
	SetVisibility(ESlateVisibility::Hidden);
	return true;
}

void UCharacterSelectWidget::UpdateInfo(FCharacterSelectedInfo CharacterSelectInfo)
{
	if (CharacterSelectInfo.bIsSelected)
	{
		SetVisibility(ESlateVisibility::Visible);
		PlayerNameText->Text = FText::FromString(CharacterSelectInfo.PlayerName);
	} else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
