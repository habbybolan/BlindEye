// Copyright (C) Nicholas Johnson 2022


#include "UI/JoinLobbyScreen.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/SessionRow.h"

bool UJoinLobbyScreen::Initialize()
{
	//JoinSelectedSessionButton->OnClicked.AddDynamic(this, &UJoinLobbyScreen::OnJoinSelectedSession);
	//RefreshListButton->OnClicked.AddDynamic(this, &UJoinLobbyScreen::OnRefreshSessionList);
	return true;
}

void UJoinLobbyScreen::InitializeSessionList(TArray<FServerData> ServerDataList)
{
	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu::InitializeSessionsList] %i"), ServerDataList.Num());
	if (ScrollSessionList == nullptr) return;

	UWorld* World = this->GetWorld();
	if (World == nullptr) return;

	ScrollSessionList->ClearChildren();
	uint32 IndexRow = 0;
	for (const FServerData& ServerData : ServerDataList)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UMainMenu::InitializeSessionsList] %s"), *ServerData.Name);

		USessionRow* Row = CreateWidget<USessionRow>(World, SessionRowType);
		if (Row == nullptr) return;

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		// Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
		//
		// FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		// Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		//
		// Row->Setup(this, IndexRow);
		++IndexRow;
		ScrollSessionList->AddChild(Row);
	}
}

void UJoinLobbyScreen::OnJoinSelectedSession()
{
	// Try joining the currently selected session index
	SessionMenuInterface->JoinSession(0);
}

void UJoinLobbyScreen::OnRefreshSessionList()
{
	// reload list of sessions
	SessionMenuInterface->RefreshSessionList();
}
