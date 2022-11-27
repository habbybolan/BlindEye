// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/CharacterSelectScreen.h"
#include "UI/LobbyScreenBase.h"
#include "BlindEyeGameInstance.generated.h"

class UCreateSessionCallbackProxy;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UBlindEyeGameInstance : public UGameInstance, public ISessionMenuInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULobbyScreenBase> CreateLobbyType;
	UFUNCTION(BlueprintCallable)
	void LoadCreateLobby();

	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<ULobbyScreenBase> JoinLobbyType;
	UFUNCTION(BlueprintCallable)
	void LoadJoinLobby();

	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<UCharacterSelectScreen> CharacterSelectType;
	UFUNCTION(BlueprintCallable)
	void LoadCharacterSelect();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingScreenType;

	IOnlineSessionPtr SessionInterface;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULobbyScreenBase> MainMenuType;
	
	UPROPERTY()
	ULobbyScreenBase* LobbyScreenBase;

	UPROPERTY() 
	UCharacterSelectScreen* CharacterSelectScreenBase;

	// Shareable pointer to the session
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	virtual void Init() override;

	virtual void Host(FString ServerName) override;
	virtual void JoinSession(uint32 Index) override;
	virtual void EndSession() override;

	virtual void RefreshSessionList() override;

	void OnFindSessionsComplete(bool success);
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySessionComplete(FName SessionName, bool SuccessfullyClosed);

	void CreateSession();

	virtual FString GetLobbyName() override;

	virtual void SetIsLAN(bool IsLan) override;

	// Enter game from character select screen
	void EnterGame(FString crowPlayerID, FString phoenixPlayerID);
	void EnterGameLAN(EPlayerType hostType, EPlayerType clientType); 

	const FName SESSION_NAME = TEXT("BlindEyeServer");

	EPlayerType GetPlayerType(APlayerState* PlayerState);

	// if game being played in editor (No Sessions created)
	bool bInEditor = true;
	bool bIsHost = false;

	FOnDestroySessionCompleteDelegate DestroyDelegate;

	void OnPlayerChanged(bool bJoined);

	void AddLoadingScreen();

protected:
 
	FString CrowPlayerID;
	FString PhoenixPlayerID;
	
	EPlayerType HostType;
	EPlayerType ClientType;

	bool bLANGame = false;
	FString JoinedSessionName;
};
