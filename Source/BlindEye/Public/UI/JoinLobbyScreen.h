// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UI/LobbyScreenBase.h"
#include "JoinLobbyScreen.generated.h"

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

protected:

	UPROPERTY(meta = (BindWidget))
	UButton* JoinSelectedSessionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RefreshListButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollSessionList;
	
	TOptional<uint32> SelectedScrollIndex = 0;

	virtual void InitializeSessionList(TArray<FServerData> ServerDataList) override;

	UFUNCTION()
	void OnJoinSelectedSession();

	UFUNCTION()
	void OnRefreshSessionList();

	void SelectSession(uint8 SessionIndex);

	USessionRow* GetSessionAtIndex(uint8 SessionIndex);
	
};
