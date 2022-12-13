// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Burrower/BurrowerSpawnManager.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Hunter/HunterEnemyController.h"
#include "DifficultyManager.generated.h"

class ABlindEyeGameState;
class AIslandManager;
class AIsland;

USTRUCT()
struct FBurrowerSpawningInfo
{
	GENERATED_BODY()

	float RemainingTime;
	UPROPERTY()
	AIsland* Island;
	bool bFirstSpawn = true;
};

class UTimelineComponent;
UCLASS()
class BLINDEYE_API ADifficultyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ADifficultyManager();

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category=Burrower, meta=(ClampMin=0, ToolTip="Spawn rate for base Burrower spawning speed at each island that's multiplied by multiplier curve to update the spawn rate as the round progresses")) 
	TArray<float> BurrowerBaseSpawnDelayPerRound;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category=Burrower, meta=(ClampMin=0, ToolTip="spawn delay for first burrower spawn at each island when a new round starts")) 
	TArray<float> BurrowerInitialSpawnDelayPerRound; 
 
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category=Burrower, meta=(ToolTip="Multiplier of burrower base spawn rate for the round to incr/decr spawn rates as round progresses")) 
	TArray<UCurveFloat*> BurrowerSpawnMultiplierPerRoundCurve;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = 0, Category=Burrower, ToolTip="Seconds of variability to add to the burrower spawn timer so each island's spawn times are offset"))
	float BurrowerSpawnVariabilityLengthen = 5; 

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, Category=Burrower, ToolTip="Seconds of variability to remove from the burrower spawn timer so each island's spawn times are offset"))
	float BurrowerSpawnVariabilityShorten = 5;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, Category=Hunter, ToolTip="Spawn rate for base hunter spawning speed that's multiplied by multiplier curve to update the spawn rate as the game progresses")) 
	float HunterBaseSpawnRate = 25;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category=Hunter, meta=(ClampMin=0, ToolTip="spawn delay for first Hunter spawn when a new round starts"))  
	TArray<float> HunterInitialSpawnDelayPerRound;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = 0, Category=Hunter, ToolTip="Seconds of variability to add to the INITIAL hunter spawn timer"))
	float HunterInitialSpawnVariabilityLengthen = 5; 
 
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, Category=Hunter, ToolTip="Seconds of variability to remove the INITIAL hunter spawn timer"))
	float HunterInitialSpawnVariabilityShorten = 5;

	UPROPERTY(EditDefaultsOnly, Category=Hunter, meta=(ToolTip="Multiplier of Hunter base spawn rate for the entire game to incr/decr spawn rates as the game progresses")) 
	UCurveFloat* HunterSpawnMultiplierCurve;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ToolTip="Time in seconds for remaining time of each round until the burst wave happens. If 10, then the last 10 seconds of the round will be a burst wave"))
	TArray<float> BurstWaveDurationPerRound; 
	
protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	ABlindEyeGameState* BlindEyeGS;

	UPROPERTY()
	AIslandManager* IslandManager;

	UPROPERTY()
	ABurrowerSpawnManager* BurrowerSpawnManager;

	UPROPERTY()
	AHunterEnemyController* HunterController;
   
	FOnTimelineFloat BurrowerTimerTimelineFloat;
	FOnTimelineFloat HunterTimerTimelineFloat; 
	UFUNCTION()
	void BurrowerSpawnTimer(float Value);
	UFUNCTION()
	void HunterSpawnTimer(float Value); 
	
	UPROPERTY()
	UTimelineComponent* CurrBurrowerSpawnMultTimeline;
	UPROPERTY()
	UTimelineComponent* HunterSpawnMultTimeline; 
	
	UPROPERTY()
	TArray<FBurrowerSpawningInfo> IslandSpawnInfo;

	float CurrBurrowerSpawnMultiplier;

	float CurrHunterSpawnTime = 0;
	bool bIsFirstHunterSpawn = true;

	bool bInBurstWave = false;

	FTimerHandle BurstWaveTimerHandle;
	void PerformBurstWave(); 
	void StopBurstWave();  

	UFUNCTION()
	void OnGameStarted();
	UFUNCTION() 
	void OnGameEnding();

	UFUNCTION() 
	void OnNewRound(uint8 CurrRound, float roundLength);

	void PlayBurrowerSpawnTimelines(uint8 CurrRound, float roundLength);
	void PlayHunterSpawnTimeline();

	// Add island to keep track of for spawning purposes
	UFUNCTION()
	void IslandAdded(AIsland* Island);

	void ResetBurrowerSpawnTimer(FBurrowerSpawningInfo& SpawnInfo, uint8 CurrRound);
	void ResetHunterSpawnTimer(uint8 CurrRound);

	UFUNCTION()
	void GameTimeSkipped(float TimeSkipped);
	

};

