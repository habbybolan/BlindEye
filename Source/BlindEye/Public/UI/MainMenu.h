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
class USessionRow;

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

	UPROPERTY()
	TScriptInterface<ISessionMenuInterface> SessionMenuInterface;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USessionRow> SessionRowType;

	virtual void NativeConstruct() override;

	virtual bool Initialize() override;

	void InitializeSessionList(TArray<FServerData> ServerData);

	UFUNCTION()
	void OnNewSessionPressed();
	UFUNCTION()
	void OnJoinSelectedSession();

	void TearDown();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinSelectedSessionButton;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollSessionList;
	
	TOptional<uint32> SelectedScrollIndex = 0;
	
};
