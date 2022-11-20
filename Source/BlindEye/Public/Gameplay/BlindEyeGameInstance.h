// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/LobbyScreenBase.h"
#include "BlindEyeGameInstance.generated.h"

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

	IOnlineSessionPtr SessionInterface;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULobbyScreenBase> MainMenuType;
	
	UPROPERTY()
	ULobbyScreenBase* LobbyScreenBase;

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

	void CreateSession(); 

	// Enter game from character select screen
	void EnterGame(FString crowPlayerID, FString phoenixPlayerID);

	const FName SESSION_NAME = TEXT("BlindEyeServer");

	EPlayerType GetPlayerType(FString PlayerID);

protected:
 
	FString CrowPlayerID;
	FString PhoenixPlayerID;
};
