// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/SessionMenuInterface.h"
#include "UI/MainMenu.h"
#include "BlindEyeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBlindEyeGameInstance : public UGameInstance, public ISessionMenuInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	IOnlineSessionPtr SessionInterface;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenu> MainMenuType;
	
	UPROPERTY()
	UMainMenu* MainMenu;

	// Shareable pointer to the session
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	virtual void Init() override;

	virtual void Host(FString ServerName) override;
	virtual void JoinSession(uint32 Index) override;
	virtual void EndSession() override;

	void OpenSessionListMenu();

	void OnFindSessionsComplete(bool success);

	void OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	const FName SESSION_NAME = TEXT("BlindEyeServer");
};
