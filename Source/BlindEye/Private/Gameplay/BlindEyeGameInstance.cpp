// Copyright (C) Nicholas Johnson 2022

#include "Gameplay/BlindEyeGameInstance.h"

#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../../Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystemTypes.h"
#include "../../Engine/Plugins/Online/OnlineSubsystemUtils/Source/OnlineSubsystemUtils/Classes/CreateSessionCallbackProxy.h"
#include "CharacterSelect/CharacterSelectGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


const static FName SESSION_NAME = TEXT("BlindEyeGame");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

void UBlindEyeGameInstance::LoadCreateLobby()
{
	LobbyScreenBase = CreateWidget<ULobbyScreenBase>(this, CreateLobbyType);
	LobbyScreenBase->AddToViewport();
	LobbyScreenBase->SessionMenuInterface = this;
}

void UBlindEyeGameInstance::LoadJoinLobby()
{
	LobbyScreenBase = CreateWidget<ULobbyScreenBase>(this, JoinLobbyType);
	LobbyScreenBase->AddToViewport();
	LobbyScreenBase->SessionMenuInterface = this;
}

void UBlindEyeGameInstance::LoadCharacterSelect()
{
	CharacterSelectScreenBase = CreateWidget<UCharacterSelectScreen>(this, CharacterSelectType);
	CharacterSelectScreenBase->AddToViewport();
	CharacterSelectScreenBase->SetSessionMenuInterface(this);
	
	if (bIsHost)
		CharacterSelectScreenBase->NotifyPlayersInSessionUpdated(1);
	else
		CharacterSelectScreenBase->NotifyPlayersInSessionUpdated(2);
}

void UBlindEyeGameInstance::Init()
{
	Super::Init();

	// Declare of SessionInterface for the subsystem
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	SessionInterface = SubSystem->GetSessionInterface();
	if (SessionInterface.IsValid()) 
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnFindSessionsComplete);     
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBlindEyeGameInstance::OnJoinSessionsComplete);
	}

	DestroyDelegate.BindUObject(this, &UBlindEyeGameInstance::OnDestroySessionComplete);
}

void UBlindEyeGameInstance::Host(FString ServerName)
{
	JoinedSessionName = ServerName;

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
		UE_LOG(LogTemp, Warning, TEXT("[UBlindEyeGameInstance::OnCreateSessionComplete] UNSUCESS"));
		LobbyScreenBase->LoadingFailed("Failed to Create a session");
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UBlindEyeGameInstance::OnCreateSessionComplete] SUCESS SessionName: %s"), *SessionName.ToString());

	// Teardown Menu and change levels
	if (LobbyScreenBase != nullptr)
	{
		LobbyScreenBase->TearDown();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete::OnCreateSessionComplete] HOST TRAVEL TO LOBBY"));

	bIsHost = true;
	//bUseSeamlessTravel = true;
	LobbyScreenBase->LoadingSucceeded();
	SER_LoadCharacterSelectMap();
}

void UBlindEyeGameInstance::SER_LoadCharacterSelectMap_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	World->ServerTravel("/Game/Maps/CharacterSelectMap?listen");
}

void UBlindEyeGameInstance::JoinSession(uint32 Index)
{
	// SESSION_NAME is a constant for the sessions
	// const static FName SESSION_NAME = TEXT("CoopPuzzleGameSession");
	UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::JoinSession] Checking if valid index: %i"), Index);
	if (Index < (uint32)(SessionSearch->SearchResults.Num()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::JoinSession] Joining a session"));
		SessionSearch->SearchResults[Index].Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, JoinedSessionName);
		SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
		LobbyScreenBase->LoadingStarted();
	} else
	{
		LobbyScreenBase->LoadingFailed("No Sessions to join");
	}
}

void UBlindEyeGameInstance::EndSession()
{
	SessionInterface->DestroySession(*JoinedSessionName, DestroyDelegate);
}

void UBlindEyeGameInstance::RefreshSessionList()
{
	// Create the pointer  
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::OpenSessionListMenu] Start finding session lists"));
		// Set properties
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true,
								  EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
		LobbyScreenBase->LoadingStarted();
	}
}

void UBlindEyeGameInstance::OnFindSessionsComplete(bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::OnFindSessionsComplete] Started"));
	if (Success && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::OnFindSessionsComplete] Success"));
		if (SessionSearch->SearchResults.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::OnFindSessionsComplete] Num Sessions: %s"), *FString::FromInt((uint8)SessionSearch->SearchResults.Num()));
			TArray<FServerData> ServerData;
			for (const FOnlineSessionSearchResult& SearchResult : 
									   SessionSearch->SearchResults)
			{
				FServerData Data;
				FString ServerName;
				if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
				{
					Data.Name = ServerName;
				}
				// Fill information for the menuData.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
				Data.HostUsername = SearchResult.Session.OwningUserName;
				ServerData.Add(Data);
			}
			// Send the information back to the menu
			LobbyScreenBase->InitializeSessionList(ServerData);
		}
		LobbyScreenBase->LoadingSucceeded();
	} else
	{
		LobbyScreenBase->LoadingFailed("Failed to search for sessions");
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
	UE_LOG(LogTemp, Warning, TEXT("[BlindEyeGameInstance::JoinSession] Travelling to joined session"));
	// The player controller travels to that url on that specific session
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	bIsHost = false;
	LobbyScreenBase->LoadingSucceeded();
	PlayerController->ClientTravel(Url, ETravelType::TRAVEL_Absolute);
}

void UBlindEyeGameInstance::OnDestroySessionComplete(FName SessionName, bool SuccessfullyClosed)
{
	AddLoadingScreen();
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UBlindEyeGameInstance::CreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("[UBlindEyeGameInstance::CreateSession] Creating %s"), *SESSION_NAME.ToString());

	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		// Switch between bIsLANMatch when using NULL subsystem
		if (bLANGame || IOnlineSubsystem::Get()->GetSubsystemName().ToString() == "NULL")
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Cyan, "[UBlindEyeGameInstance::CreateSession]: LAN");
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Cyan, "[UBlindEyeGameInstance::CreateSession]: ONLINE");
			SessionSettings.bIsLANMatch = false;
		}

		// Number of sessions
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, JoinedSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
		LobbyScreenBase->LoadingStarted();
	}
}

FString UBlindEyeGameInstance::GetLobbyName()
{
	return JoinedSessionName;
}

void UBlindEyeGameInstance::SetIsLAN(bool IsLan)
{
	bLANGame = IsLan;
}

void UBlindEyeGameInstance::EnterGame(FString crowPlayerID, FString phoenixPlayerID)
{
	CrowPlayerID = crowPlayerID;
	PhoenixPlayerID = phoenixPlayerID;
	if (UWorld* World = GetWorld())
	{
		bInEditor = false;
		World->ServerTravel("/Game/Maps/WhiteBox?listen");
	}
}

void UBlindEyeGameInstance::EnterGameLAN(EPlayerType hostType, EPlayerType clientType)
{
	bInEditor = false;
	HostType = hostType;
	ClientType = clientType;
}

EPlayerType UBlindEyeGameInstance::GetPlayerType(APlayerState* PlayerState)
{
	FString PlayerID = PlayerState->GetUniqueId().ToString();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "[UBlindEyeGameInstance::GetPlayerType] ID " + PlayerID);
	if (CrowPlayerID == PlayerID) return EPlayerType::CrowPlayer;
	else return EPlayerType::PhoenixPlayer;
}

void UBlindEyeGameInstance::OnPlayerChanged(bool bJoined)
{
	if (CharacterSelectScreenBase == nullptr) return;
	// notify host that other player joined
	if (bJoined)
		CharacterSelectScreenBase->NotifyPlayersInSessionUpdated(2);
	else
		CharacterSelectScreenBase->NotifyPlayersInSessionUpdated(1);
}

void UBlindEyeGameInstance::AddLoadingScreen()
{
	UUserWidget* Widget =  CreateWidget(this, LoadingScreenType);
	Widget->AddToViewport();
}

void UBlindEyeGameInstance::CharacterSelectFadeIntoBlack(float Duration)
{
	if (CharacterSelectScreenBase != nullptr)
	{
		CharacterSelectScreenBase->FadeIntoBlack(Duration);
	}
}
