// Copyright (C) Nicholas Johnson 2022

#include "Gameplay/BlindEyeGameInstance.h"

#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystemTypes.h"
#include "Kismet/GameplayStatics.h"


const static FName SESSION_NAME = TEXT("BlindEyeGame");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

void UBlindEyeGameInstance::LoadMainMenu()
{
	UWorld* World = GetWorld();
	MainMenu = CreateWidget<UMainMenu>(GetLocalPlayerByIndex(0)->GetPlayerController(World), MainMenuType);
	MainMenu->AddToViewport();
	MainMenu->SessionMenuInterface = this;
}

void UBlindEyeGameInstance::Init()
{
	Super::Init();

	// Declare of SessionInterface for the subsystem
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	SessionInterface = SubSystem->GetSessionInterface();
	if (SessionInterface.IsValid()) 
	{
		// Subscribe to minimum events to handling sessions
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnCreateSessionComplete);
		//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnFindSessionsComplete);     
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnJoinSessionsComplete);
	}
}

void UBlindEyeGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;

	if (SessionInterface.IsValid())
	{
		// Checks for an existing session
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (ExistingSession != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::Host] There is an existing session about to remove the current one"));			

			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::Host] About to create session"));			

			// Create a new session
			CreateSession();
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::Host] SessionInterface invalid"));
	}
}

void UBlindEyeGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	// It will not be success if there are more than one session with the same name already created
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::OnCreateSessionComplete] UNSUCESS"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UNetTileMazeGameInstance::OnCreateSessionComplete] SUCESS SessionName: %s"), *SessionName.ToString());

	// Teardown Menu and change levels
	if (MainMenu != nullptr)
	{
		MainMenu->TearDown();
	}

	UEngine* Engine = GetEngine();

	if (Engine == nullptr) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("[OnCreateSessionComplete::Host]"));

	UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete::OnCreateSessionComplete] HOST TRAVEL TO LOBBY"));

	UWorld* World = GetWorld();

	if (World == nullptr) return;
 
	//bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/WhiteBox?listen");
}


void UBlindEyeGameInstance::JoinSession(uint32 Index)
{
	// SESSION_NAME is a constant for the sessions
	// const static FName SESSION_NAME = TEXT("CoopPuzzleGameSession");
	if (Index < (uint32)(SessionSearch->SearchResults.Num()))
	{
		SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	}
}

void UBlindEyeGameInstance::EndSession()
{
	
}

void UBlindEyeGameInstance::OpenSessionListMenu()
{
	// Create the pointer  
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		// Set properties
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true,
								  EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0,
			SessionSearch.ToSharedRef());
	}
}

void UBlindEyeGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num() > 0)
		{
			TArray<FServerData> ServerData;
			for (const FOnlineSessionSearchResult& SearchResult : 
									   SessionSearch->SearchResults)
			{
				FServerData Data;
				FString ServerName;
				if (SearchResult.Session.SessionSettings.Get("SERVER_NAME_SETTINGS_KEY", ServerName))
				{
					Data.Name = ServerName;
				}
				// Fill information for the menuData.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
				Data.HostUsername = SearchResult.Session.OwningUserName;
				ServerData.Add(Data);
			}
			// Send the information back to the menu
			MainMenu->InitializeSessionList(ServerData);
		}
	}
}

void UBlindEyeGameInstance::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// Get the url for that session first
	FString Url;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Url))
	{
		return;
	}
	// The player controller travels to that url on that specific session
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	PlayerController->ClientTravel(Url, ETravelType::TRAVEL_Absolute);
}

void UBlindEyeGameInstance::CreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::CreateSession] Creating %s"), *SESSION_NAME.ToString());

	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		// Switch between bIsLANMatch when using NULL subsystem
		if (IOnlineSubsystem::Get()->GetSubsystemName().ToString() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}

		// Number of sessions
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}
