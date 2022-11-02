// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Islands/IslandManager.h"
#include "Shrine.h"
#include "GameFramework/GameState.h"
#include "BlindEyeGameState.generated.h"

UENUM(BlueprintType)
enum class EGameOverState : uint8
{
	InProgress, 
	Won,
	Lost
};

class ABlindEyePlayerCharacter;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameState : public AGameState
{
	GENERATED_BODY()

public:

	ABlindEyeGameState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialStartedSignature);
	FTutorialStartedSignature TutorialStartedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialEndedSignature);
	FTutorialEndedSignature TutorialEndedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStartedSignature);
	FGameStartedSignature GameStartedDelegate; 

	ABlindEyePlayerCharacter* GetRandomPlayer();

	UFUNCTION(BlueprintCallable)
	AShrine* GetShrine();

	AIslandManager* GetIslandManager();
	
	UPROPERTY(Replicated)
	bool bWinConditionPaused = false;

	// debugger flag for making hunter visible
	UPROPERTY(Replicated)
	bool bHunterAlwaysVisible = false;

	UPROPERTY(Replicated)
	EGameOverState GameOverState = EGameOverState::InProgress;
	
	virtual void SetInProgressMatchState(FName NewInProgressState);

	// getters for player characters to check the state of the game
	bool IsBlindEyeMatchNotInProgress(); 
	bool IsBlindEyeMatchWaitingPlayers();
	bool IsBlindEyeMatchTutorial();
	bool IsBlindEyeMatchInProgress();
	bool IsBlindEyeMatchEnding();

	void TutorialFinished();
	void StartGame();
	
	TArray<ABlindEyePlayerCharacter*> GetPlayers();
	ABlindEyePlayerCharacter* GetPlayer(EPlayerType PlayerType);

	// Send from GameMode to update the game timers
	void UpdateMainGameTimer(float GameTimer);

	// Keeps track of main loop timers, which are periodically synced from server. Doesn't run any game logic
	UFUNCTION()
	void RunMainGameLoopTimers();

	void SetGameTime(float timerUntilGameWon); 

protected:
	TWeakObjectPtr<AShrine> Shrine;

	UPROPERTY(ReplicatedUsing=OnRep_InProgressMatchState, BlueprintReadOnly, VisibleInstanceOnly, Category = GameState)
	FName InProgressMatchState;

	UPROPERTY()
	AIslandManager* IslandManager;

	UPROPERTY(Replicated)
	float CurrGameTime;
	float TimerUntilGameWon;  
	FTimerHandle MainGameLoopTimer;
	float MainGameLoopDelay = 0.1;

	void GameInProgressState();
	UFUNCTION()
	virtual void OnRep_InProgressMatchState();

	void TutorialState();
	
};
