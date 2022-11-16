// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

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
/**
 * 
 */
UCLASS()
class BLINDEYE_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UButton* NewSessionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	uint32 SelectedScrollIndex = 0;

	UPROPERTY()
	TScriptInterface<ISessionMenuInterface> SessionMenuInterface;

	virtual void NativeConstruct() override;

	void InitializeSessionList(TArray<FServerData> ServerData);

	void OnNewSessionPressed();
	void OnJoinSelectedSession();
	
};
