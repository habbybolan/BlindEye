// Copyright (C) Nicholas Johnson 2022


#include "UI/JoinLobbyScreen.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/SessionRow.h"

bool UJoinLobbyScreen::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	JoinSelectedSessionButton->OnClicked.AddDynamic(this, &UJoinLobbyScreen::OnJoinSelectedSession);
	RefreshListButton->OnClicked.AddDynamic(this, &UJoinLobbyScreen::OnRefreshSessionList);
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
	// TODO:
	//SessionMenuInterface->JoinSession(0);

	// if (SelectedScrollIndex.IsSet())
	// {
	// 	int32 ScrollCount = ScrollSessionList->GetChildrenCount();
	// 	int32 SelectedIndex = (int32)SelectedScrollIndex.GetValue();
	// 	if ((ScrollCount > 0) && (SelectedIndex >= 0) && (SelectedIndex < ScrollCount))
	// 	{
	// 		SessionMenuInterface->JoinSession(SelectedScrollIndex.GetValue());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("[UMainMenu::InitializeSessionsList] No sessions available"));
	// 	}
	// 	
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu::InitializeSessionsList] Unable to Join Session"));
	// }
}

void UJoinLobbyScreen::OnRefreshSessionList()
{
	// reload list of sessions
	SessionMenuInterface->RefreshSessionList();

	if ((ScrollSessionList == nullptr) && (SessionMenuInterface == nullptr)) return;
}

void UJoinLobbyScreen::SelectSession(uint8 SessionIndex)
{
	if (SelectedScrollIndex.IsSet())
	{
		USessionRow* CurrSelectedSession = GetSessionAtIndex(SelectedScrollIndex.GetValue());
		CurrSelectedSession->SetSelected(false);
	}
	USessionRow* Session = GetSessionAtIndex(SessionIndex);
	Session->SetSelected(true);
}

USessionRow* UJoinLobbyScreen::GetSessionAtIndex(uint8 SessionIndex)
{
	check(SessionIndex < ScrollSessionList->GetChildrenCount())
	USessionRow* SessionRow = Cast<USessionRow>(ScrollSessionList->GetChildAt(SessionIndex));
	check(SessionRow)
	return SessionRow;
}
