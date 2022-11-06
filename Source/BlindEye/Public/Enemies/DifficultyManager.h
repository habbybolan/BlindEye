// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Burrower/BurrowerSpawnManager.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
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
};

class UTimelineComponent;
UCLASS()
class BLINDEYE_API ADifficultyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ADifficultyManager();

	UPROPERTY(EditDefaultsOnly) 
	TArray<float> BurrowerBaseSpawnDelayPerRound;
 
	UPROPERTY(EditDefaultsOnly) 
	TArray<UCurveFloat*> BurrowerSpawnMultiplierPerRoundCurve;

	UPROPERTY(EditDefaultsOnly)
	float HunterBaseSpawnRate;

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
   
	FOnTimelineFloat BurrowerTimerTimelineFloat;
	UFUNCTION()
	void BurrowerSpawnTimer(float Value);
	
	UPROPERTY()
	UTimelineComponent* CurrBurrowerSpawnMultTimeline;
	UPROPERTY()
	TArray<FBurrowerSpawningInfo> IslandSpawnInfo;

	float CurrBurrowerSpawnMultiplier;

	UFUNCTION()
	void OnGameStarted();

	UFUNCTION() 
	void OnNewRound(uint8 CurrRound, float roundLength);

	void PlayBurrowerSpawnTimelines(uint8 CurrRound, float roundLength);
	void SetupHunterSpawnTimeline();

	// Add island to keep track of for spawning purposes
	UFUNCTION()
	void IslandAdded(AIsland* Island);

	void ResetSpawnTimer(FBurrowerSpawningInfo& SpawnInfo, uint8 CurrRound);

	UFUNCTION()
	void GameTimeSkipped(float TimeSkipped);
	

};

