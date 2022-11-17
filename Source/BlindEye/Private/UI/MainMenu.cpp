// Copyright (C) Nicholas Johnson 2022


#include "UI/MainMenu.h"

#include "Characters/BlindEyePlayerController.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/SessionRow.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	NewSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnNewSessionPressed);
	JoinSelectedSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinSelectedSession);
	RefreshListButton->OnClicked.AddDynamic(this, &UMainMenu::OnRefreshSessionList);
	return true;
}

void UMainMenu::InitializeSessionList(TArray<FServerData> ServerDataList)
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

void UMainMenu::OnNewSessionPressed()
{
	if (SessionMenuInterface == nullptr) return;
	SessionMenuInterface->Host("BlindEyeServer");
}

void UMainMenu::OnJoinSelectedSession()
{
	// Calling the method Join Session with a specific index
	// SelectedScrollIndex is an uint32 representing the row on a scroll // rect
	SessionMenuInterface->JoinSession(0);
}

void UMainMenu::OnRefreshSessionList()
{
	// reload list of sessions
	SessionMenuInterface->RefreshSessionList();
}

void UMainMenu::TearDown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	ABlindEyePlayerController* PlayerController = Cast<ABlindEyePlayerController>(World->GetFirstPlayerController());
	if (PlayerController == nullptr) return;

	// Set the Input Mode for game mode: allows only the player input / player controller to respond to user input.
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	//PlayerController->UnLockInput();
	PlayerController->bShowMouseCursor = false;
}
