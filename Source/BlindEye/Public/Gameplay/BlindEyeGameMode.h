// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/GameMode.h"
#include "Islands/IslandManager.h"
#include "BlindEyeGameMode.generated.h"

class AHunterEnemyController;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameMode : public AGameModeBase
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

	// called by shrine when it's destroyed
	void OnShrineDeath();

	virtual void RestartGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemyController> HunterControllerType;

	void PauseWinCondition(bool IsPauseWinCond);

	void IncrementTimeByAMinute();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_LevelShift();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Pulse(uint8 PulseIndex);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	AIslandManager* IslandManager;

	float CurrIslandLevelTime = 0;

	uint8 CurrPulseIndex = 0;
	uint8 NumPulses = 3;
	float TimeBetweenPulses;

	FTimerHandle PulseKillDelayTimerHandle;
	
	void OnGameEnded();
	void OnGameWon();

	void PerformPulse();
 
	float GameTimer = 0;

	virtual void Tick(float DeltaSeconds) override;
	
	
};
