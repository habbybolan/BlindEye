// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "TutorialBase.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class ATutorialBase;

UCLASS()
class BLINDEYE_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialManager();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float DelayBetweenTutorials = 1;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATutorialBase>> TutorialTypes;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNextTutorialStarted);
	FNextTutorialStarted NextTutorialStartedDelegate;

	void StartTutorials();
	UFUNCTION()
	void GotoNextTutorial();

	void SetFinishTutorials();

	// Player requesting player type info of currently active tutorial
	TArray<FTutorialInfo> GetCurrentTutorialInfo(EPlayerType PlayerType);

protected:

	UPROPERTY()
	TArray<ATutorialBase*> AllTutorials;

	uint8 CurrTutorialIndex = 0;
	bool bTutorialsRunning = false;

	bool bTutorialEnded = false;

	FTimerHandle DelayBetweenTutorialsTimerHandle;

	void StartNextTutorial();

	UFUNCTION()
	void NewPlayerJoined(AGameModeBase* GameModeBase, APlayerController* NewPlayer);

};
