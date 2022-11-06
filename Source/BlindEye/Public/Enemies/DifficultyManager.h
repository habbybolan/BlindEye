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

	UPROPERTY(EditDefaultsOnly, EditFixedSize) 
	TArray<float> BurrowerBaseSpawnDelayPerRound;

	UPROPERTY(EditDefaultsOnly, EditFixedSize) 
	TArray<float> BurrowerInitialSpawnDelayPerRound; 
 
	UPROPERTY(EditDefaultsOnly, EditFixedSize) 
	TArray<UCurveFloat*> BurrowerSpawnMultiplierPerRoundCurve;

	UPROPERTY(meta=(ClampMin = 0, ToolTip="Seconds of variability to add to the burrower spawn timer so each island's spawn times are offset"))
	float BurrowerSpawnVariabilityLengthen = 5; 

	UPROPERTY(meta=(ClampMin=0, ToolTip="Seconds of variability to remove from the burrower spawn timer so each island's spawn times are offset"))
	float BurrowerSpawnVariabilityShorten = 5;

	UPROPERTY(EditDefaultsOnly)
	float HunterBaseSpawnRate = 25;

	UPROPERTY(EditDefaultsOnly) 
	UCurveFloat* HunterSpawnMultiplierCurve;
	
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

	UFUNCTION()
	void OnGameStarted();

	UFUNCTION() 
	void OnNewRound(uint8 CurrRound, float roundLength);

	void PlayBurrowerSpawnTimelines(uint8 CurrRound, float roundLength);
	void SetupHunterSpawnTimeline();

	// Add island to keep track of for spawning purposes
	UFUNCTION()
	void IslandAdded(AIsland* Island);

	void ResetBurrowerSpawnTimer(FBurrowerSpawningInfo& SpawnInfo, uint8 CurrRound);

	UFUNCTION()
	void GameTimeSkipped(float TimeSkipped);
	

};

