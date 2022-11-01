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
class BLINDEYE_API ABlindEyeGameState : public AGameStateBase
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

	TArray<ABlindEyePlayerCharacter*> GetPlayers();
	ABlindEyePlayerCharacter* GetPlayer(EPlayerType PlayerType);

protected:
	TWeakObjectPtr<AShrine> Shrine;

	UPROPERTY()
	AIslandManager* IslandManager;

	
	
	
};
