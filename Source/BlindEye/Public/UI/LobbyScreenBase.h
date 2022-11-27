// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyScreenBase.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

struct FServerData;
class ISessionMenuInterface;
class USessionMenuInterface;
class UButton;
class USessionRow;
class UScrollBox;

/**
 * Base screen for joining/creating multiplayer sessions
 */
UCLASS(Abstract)
class BLINDEYE_API ULobbyScreenBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TScriptInterface<ISessionMenuInterface> SessionMenuInterface;

	virtual void NativeConstruct() override;

	virtual bool Initialize() override;

	virtual void InitializeSessionList(TArray<FServerData> ServerDataList);

	virtual void LoadingFailed(FString Message) PURE_VIRTUAL(ULobbyScreenBase,);
	virtual void LoadingStarted() PURE_VIRTUAL(ULobbyScreenBase,);
	virtual void LoadingSucceeded() PURE_VIRTUAL(ULobbyScreenBase,);

	UFUNCTION(BlueprintCallable)
	void TearDown();
	
};
