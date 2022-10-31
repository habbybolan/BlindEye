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
	bool IsBlindEyeMatchWaitingPlayers();
	bool IsBlindEyeMatchTutorial();
	bool IsBlindEyeMatchInProgress();
	bool IsBlindEyeMatchEnding();

protected:
	TWeakObjectPtr<AShrine> Shrine;

	UPROPERTY(ReplicatedUsing=OnRep_InProgressMatchState, BlueprintReadOnly, VisibleInstanceOnly, Category = GameState)
	FName InProgressMatchState;

	UPROPERTY()
	AIslandManager* IslandManager;
	
	UFUNCTION()
	virtual void OnRep_InProgressMatchState();
	
};
