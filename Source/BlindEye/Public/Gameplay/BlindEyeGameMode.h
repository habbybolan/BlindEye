// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/GameMode.h"
#include "Islands/IslandManager.h"
#include "BlindEyeGameMode.generated.h"

namespace InProgressStates 
{
	const FName NotInProgress = FName(TEXT("NotInProgress"));
	const FName WaitingLoadingPhase = FName(TEXT("WaitingForPlayers"));
	const FName Tutorial = FName(TEXT("Tutorial"));						
	const FName GameInProgress = FName(TEXT("GameInProgress"));						
	const FName GameEnding = FName(TEXT("GameEnding"));				
}

class AHunterEnemyController;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	FTransform GetSpawnPoint() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<ABlindEyePlayerCharacter>> PlayerClassTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimerUntilGameWon = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayBetweenLevelShifts = 60.f;

	UPROPERTY(EditDefaultsOnly)
	float PulseKillDelay = 1.0f;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialStartedSignature);
	FTutorialStartedSignature TutorialStartedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialEndedSignature);
	FTutorialEndedSignature TutorialEndedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStartedSignature);
	FGameStartedSignature GameStartedDelegate; 
	
	// called by shrine when it's destroyed
	void OnShrineDeath();

	virtual void HandleMatchHasStarted() override;

	FName InProgressMatchState;
	virtual void SetInProgressMatchState(FName NewInProgressState);
	void OnBlindEyeMatchStateSet();

	virtual void RestartGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemyController> HunterControllerType;

	void PauseWinCondition(bool IsPauseWinCond);

	void IncrementTimeByAMinute();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_LevelShift();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Pulse(uint8 PulseIndex);

	// Player notifies game mode when they finished the mechanics tutorial
	void TutorialFinished(ABlindEyePlayerCharacter* Player);

	// Called for setting game in progress and starting the main loop
	void StartGame();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	AIslandManager* IslandManager;

	float CurrIslandLevelTime = 0;

	uint8 CurrPulseIndex = 0;
	uint8 NumPulses = 3;
	float TimeBetweenPulses;

	FTimerHandle PulseKillDelayTimerHandle;

	// states **********

	void TutorialState();
	void GameInProgressState();
	void GameEndingState();

	// States Ended ****
	
	void OnGameEnded();
	void OnGameWon();

	void PerformPulse();
 
	float GameTimer = 0;

	virtual void Tick(float DeltaSeconds) override;
	
	
};
