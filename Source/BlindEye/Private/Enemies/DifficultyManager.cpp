// Copyright (C) Nicholas Johnson 2022


#include "Enemies/DifficultyManager.h"

#include "Components/TimelineComponent.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

ADifficultyManager::ADifficultyManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrBurrowerSpawnMultTimeline = CreateDefaultSubobject<UTimelineComponent>("BurrowerSpawnTimeline");
}

void ADifficultyManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (ensure(World))
	{
		BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)

		BlindEyeGS->PulseDelegate.AddDynamic(this, &ADifficultyManager::OnNewRound);

		BlindEyeGS->GameStartedDelegate.AddDynamic(this, &ADifficultyManager::OnGameStarted);

		IslandManager = Cast<AIslandManager>(UGameplayStatics::GetActorOfClass(World, AIslandManager::StaticClass()));
		check(IslandManager);

		IslandManager->IslandAddedDelegate.AddDynamic(this, &ADifficultyManager::IslandAdded);

		BurrowerSpawnManager = Cast<ABurrowerSpawnManager>(UGameplayStatics::GetActorOfClass(World, ABurrowerSpawnManager::StaticClass()));
		check(BurrowerSpawnManager)
	}
}

void ADifficultyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
 
void ADifficultyManager::OnGameStarted()
{
	for (AIsland* Island : IslandManager->GetActiveIslands())
	{
		IslandAdded(Island);
	}
	SetupHunterSpawnTimeline();

	UWorld* World = GetWorld();
	check(World)
	ABlindEyeGameMode* BlindEyeGM = Cast<ABlindEyeGameMode>(UGameplayStatics::GetGameMode(World));
	OnNewRound(0, BlindEyeGM->GetCurrRoundLength());
}

void ADifficultyManager::OnNewRound(uint8 CurrRound, float roundLength)
{
	// TODO: Set at varying starting points so they dont spawn at same time
	for (FBurrowerSpawningInfo& SpawnInfo : IslandSpawnInfo)
	{
		ResetSpawnTimer(SpawnInfo, CurrRound);
	}
	SetupBurrowerSpawnTimelines(CurrRound, roundLength);
}

void ADifficultyManager::SetupBurrowerSpawnTimelines(uint8 CurrRound, float roundLength)
{
	// Timeline for scaling the spawn rates as the round progresses
	BurrowerTimerTimelineFloat.BindDynamic(this, &ADifficultyManager::BurrowerSpawnTimer);
	CurrBurrowerSpawnMultTimeline->AddInterpFloat(BurrowerSpawnMultiplierPerRoundCurve[CurrRound], BurrowerTimerTimelineFloat);
	// Prevent going over 1 second for scaling reason
	CurrBurrowerSpawnMultTimeline->SetTimelineLength(1);
	// Scale spawner playback to match round length
	CurrBurrowerSpawnMultTimeline->SetPlayRate(1 / roundLength);
	CurrBurrowerSpawnMultTimeline->PlayFromStart();
}

void ADifficultyManager::SetupHunterSpawnTimeline()
{
	// TODO: Hunter timer for entire game
}

void ADifficultyManager::BurrowerSpawnTimer(float Value)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Green, FString::SanitizeFloat(Value));
	UWorld* World = GetWorld();
	if (World)
	{
		// Decrement timer based on delta seconds and spawn multiplier from curve
		for (FBurrowerSpawningInfo& SpawnInfo : IslandSpawnInfo)
		{
			SpawnInfo.RemainingTime -= World->GetDeltaSeconds() * Value;
			if (SpawnInfo.RemainingTime <= 0)
			{
				BurrowerSpawnManager->SpawnBurrower(SpawnInfo.Island);
				ResetSpawnTimer(SpawnInfo, BlindEyeGS->GetCurrRound());
			}
		}
	}
}

void ADifficultyManager::IslandAdded(AIsland* Island)
{
	check(BlindEyeGS) 
	uint8 CurrRound = BlindEyeGS->GetCurrRound();
	FBurrowerSpawningInfo SpawnInfo = FBurrowerSpawningInfo();
	SpawnInfo.RemainingTime = BurrowerBaseSpawnDelayPerRound[CurrRound];
	SpawnInfo.Island = Island;
	IslandSpawnInfo.Add(SpawnInfo);
}  

void ADifficultyManager::ResetSpawnTimer(FBurrowerSpawningInfo& SpawnInfo, uint8 CurrRound)
{
	SpawnInfo.RemainingTime = BurrowerBaseSpawnDelayPerRound[CurrRound];
}

