// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerController.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "GameFramework/GameState.h"
#include "BlindEyeGameState.generated.h"

class AIslandManager;
class ABlindEyePlayerState;
class AShrine;
class ABlindEyeEnemyBase;
enum class EPlayerType : uint8;
enum class EMarkerType : uint8;

UENUM(BlueprintType)
enum class EGameOverState : uint8
{
	InProgress, 
	Won,
	Lost
};

UENUM(BlueprintType)
enum class EEnemyTutorialType : uint8
{
	BurrowerSnapper,
	Hunter
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameTimeSkippedSignature, float, TimeJumpedForwards);
	FGameTimeSkippedSignature FGameTimeSkippedDelegate;   

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

	UPROPERTY(Replicated)
	float CurrGameTime = 0;
	UPROPERTY(Replicated)
	float CurrRoundTimer = 0; 
	UPROPERTY(Replicated)
	float TimerUntilGameWon; 

	UPROPERTY(Replicated)
	bool bInEnemyTutorial = false;
	UPROPERTY(Replicated)   
	bool bInEnemyTutorialSkippableSection = false;
	UPROPERTY(Replicated)
	bool bInBeginningTutorial = false;
	
	virtual void SetInProgressMatchState(FName NewInProgressState);

	// getters for player characters to check the state of the game
	bool IsBlindEyeMatchNotInProgress(); 
	bool IsBlindEyeMatchWaitingPlayers();
	bool IsBlindEyeMatchTutorial();
	bool IsBlindEyeMatchInProgress();
	bool IsBlindEyeMatchEnding();

	void TutorialFinished();

	void EnemyTutorialFinished();
	
	void StartGame();
	
	TArray<ABlindEyePlayerCharacter*> GetPlayers();
	ABlindEyePlayerCharacter* GetPlayer(EPlayerType PlayerType);

	// Send from GameMode to update the game timers 
	void UpdateMainGameTimer(float GameTimer, float RoundTimer);

	// Keeps track of main loop timers, which are periodically synced from server. Doesn't run any game logic
	UFUNCTION()
	void RunMainGameLoopTimers();

	float GetGameDonePercent();

	TArray<ABlindEyeEnemyBase*> GetAllEnemies();

	// Called whenever enemy is spawned to allow easy retrieval of all enemies alive in level
	void SubscribeToEnemy(ABlindEyeEnemyBase* Enemy);

	void OnLevelShift();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelShiftSignature);
	FLevelShiftSignature LevelShiftDelegate;
	  
	void OnPulse(uint8 currRound, float roundLength);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPulseSignature, uint8, CurrRound, float, RoundLength);
	FPulseSignature PulseDelegate; 

	uint8 GetCurrRound();

	float GetCurrRoundLength();
	float GetCurrRoundTime(); 
	float GetPercentOfRoundFinished();
 
	void SkipGameTime(float AmountToSkip);

	UPROPERTY(Replicated)
	float CurrRoundLength = 1;

	UPROPERTY(Replicated)
	uint8 NumRounds = 3; 

	UFUNCTION() 
	void OnPlayerDied(ABlindEyePlayerState* PlayerThatDied);
	UFUNCTION()
	void OnPlayerRevived(ABlindEyePlayerState* PlayerRevived);

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void NotifyOtherPlayerOfPlayerExisting(ABlindEyePlayerCharacter* NewPlayer);

	APlayerState* GetOtherPlayer(ABlindEyePlayerCharacter* Player);
 
	void PauseGameLogicAndDisplayText(EEnemyTutorialType TutorialType);

	// Notify BP to display text info of enemies
	UFUNCTION(BlueprintImplementableEvent)
	void BP_TutorialBurrowerSpawned_SER(ABurrowerEnemy* TutorialBurrower);

	void EnemyTutorialTrigger(EEnemyTutorialType TutorialType);

protected:

	UPROPERTY(Replicated)
	TWeakObjectPtr<AShrine> Shrine;

	UPROPERTY(Replicated)
	TArray<ABlindEyeEnemyBase*> AllEnemies;

	UPROPERTY(ReplicatedUsing=OnRep_InProgressMatchState, BlueprintReadOnly, VisibleInstanceOnly, Category = GameState)
	FName InProgressMatchState;

	UPROPERTY()
	AIslandManager* IslandManager;
	
	FTimerHandle MainGameLoopTimer;
	float MainGameLoopDelay = 0.1;

	UPROPERTY(Replicated)
	uint8 CurrRound = 0;

	UFUNCTION()
	void EnemyDied(AActor* Enemy);

	void GameInProgressState();
	UFUNCTION()
	virtual void OnRep_InProgressMatchState();

	void TutorialState();

	void GetShrineReference();
 
	UFUNCTION()
	void OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType);
	UFUNCTION()
	void OnMarkRemoved(AActor* UnmarkedActor, EMarkerType MarkerType);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowEnemyTextSnippet(EEnemyTutorialType TutorialType);
	
};
