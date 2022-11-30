// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/BlindEyeGameState.h"
#include "TutorialBase.generated.h"

class UScaleBox;
class ABlindEyeGameState;

USTRUCT()
struct FTutorialInfo
{
	GENERATED_BODY()
 
	void Init(uint8 id, FString textToDislay, uint8 maxCount)
	{
		ID = id;
		TextToDisplay = textToDislay;
		MaxCount = maxCount;
	}

	UPROPERTY()
	uint8 ID;
	UPROPERTY()
	FString TextToDisplay;
	UPROPERTY()
	uint8 MaxCount;
};

UCLASS(Abstract)
class BLINDEYE_API ATutorialBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialBase();

	DECLARE_DYNAMIC_DELEGATE(FTutorialFinishedSignature);
	FTutorialFinishedSignature TutorialFinishedDelegate;

	UPROPERTY(EditDefaultsOnly)
	float EndTutorialDelay = 1.5f;
 
	virtual void SetupTutorial();
	virtual void EndTutorialLogic();
	virtual void EndTutorial();

	// TODO: Logic for player joining in middle of tutorial, update their states?

	TArray<FTutorialInfo> PhoenixTutorialInfo;
	TArray<FTutorialInfo> CrowTutorialInfo;

	TArray<FTutorialInfo>& GetPlayerTutorialArray(EPlayerType PlayerType);

	void InitializePlayerForTutorial(APlayerState* Player);

	TSet<uint32> PlayerIDAlreadyBinded;

	bool IsPlayerAlreadyInTutorial(APlayerState* Player);

protected:

	bool bRunning = false;
 
	UPROPERTY()
	ABlindEyeGameState* BlindEyeGS;

	FTimerHandle DelayEndTimerHandle;
 
	void UpdateChecklistOfPlayer(EPlayerType PlayerType, uint8 ItemID);
	void AddChecklistItem(EPlayerType PlayerType, uint8 ItemID, FString& text, uint8 MaxCount);

	virtual void PlayerEnteredTutorialHelper(ABlindEyePlayerCharacter* Player) PURE_VIRTUAL(OnPlayerConnected,);
};
