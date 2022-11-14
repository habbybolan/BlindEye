// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/BlindEyeGameState.h"
#include "TutorialBase.generated.h"

class UScaleBox;
class ABlindEyeGameState;

UCLASS(Abstract)
class BLINDEYE_API ATutorialBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialBase();

	DECLARE_DYNAMIC_DELEGATE(FTutorialFinishedSignature);
	FTutorialFinishedSignature TutorialFinishedDelegate;
 
	virtual void SetupTutorial();
	virtual void EndTutorial();

	// TODO: Logic for player joining in middle of tutorial, update their states?

protected:

	bool bRunning = false;
 
	UPROPERTY()
	ABlindEyeGameState* BlindEyeGS;
 
	void UpdateChecklistOfPlayer(EPlayerType PlayerType, uint8 ItemID);
	void AddChecklistItem(EPlayerType PlayerType, uint8 ItemID, FString& text, uint8 MaxCount);
};
