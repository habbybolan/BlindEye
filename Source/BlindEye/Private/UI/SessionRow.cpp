// Copyright (C) Nicholas Johnson 2022


#include "UI/SessionRow.h"

#include "UI/LobbyScreenBase.h"

void USessionRow::SetSelected(bool IsSelected)
{
	bIsSelected = true;
	BP_SetSelected(IsSelected);
}

void USessionRow::InitializeData(FServerData Data, uint8 Index)
{
	RowIndex = Index;
	ServerName->SetText(FText::FromString(Data.Name));

	// Limit num of player name characters
	FString HostNameText = Data.HostUsername.Len() >= MaxNumHostNameCharacters ?
		Data.HostUsername.Mid(0, MaxNumHostNameCharacters) + "..."
		: Data.HostUsername;
	HostName->SetText(FText::FromString(HostNameText));

	FString FractionText = FString::Printf(TEXT("%d/%d"), Data.CurrentPlayers, Data.MaxPlayers);
	NumPlayersInLobby->SetText(FText::FromString(FractionText));

	SessionData = Data;
}

FReply USessionRow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SessionRowSelectedDelegate.ExecuteIfBound(this);
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
