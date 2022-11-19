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

	// Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
	//
	// FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
	// Row->ConnectionFraction->SetText(FText::FromString(FractionText));
	//
	// Row->Setup(this, IndexRow);

	SessionData = Data;
}

FReply USessionRow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SessionRowSelectedDelegate.ExecuteIfBound(this);
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
