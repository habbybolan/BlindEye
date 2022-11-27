// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UI/LobbyScreenBase.h"
#include "JoinLobbyScreen.generated.h"

class ULoadingIcon;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UJoinLobbyScreen : public ULobbyScreenBase
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USessionRow> SessionRowType;

	virtual void LoadingStarted() override;
	virtual void LoadingFailed(FString Message) override;
	virtual void LoadingSucceeded() override;

protected:

	UPROPERTY(meta = (BindWidget))
	UButton* JoinSelectedSessionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RefreshListButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollSessionList;

	UPROPERTY(meta = (BindWidget))
	ULoadingIcon* LoadingIcon;
	
	TOptional<uint32> SelectedScrollIndex = 0;

	virtual void InitializeSessionList(TArray<FServerData> ServerDataList) override;

	UFUNCTION()
	void OnJoinSelectedSession();

	UFUNCTION()
	void OnRefreshSessionList();

	UFUNCTION()
	void SelectSession(USessionRow* SelectedSession);

	USessionRow* GetSessionAtIndex(uint8 SessionIndex);
	
};
