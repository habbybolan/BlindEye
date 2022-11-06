// Copyright (C) Nicholas Johnson 2022


#include "Enemies/DifficultyManager.h"

#include "Components/TimelineComponent.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADifficultyManager::ADifficultyManager()
{
	PrimaryActorTick.bCanEverTick = true;

	BurrowerBaseSpawnDelayPerRound.SetNum(3);
	BurrowerBaseSpawnDelayPerRound = {20, 15, 10};

	BurrowerInitialSpawnDelayPerRound.SetNum(3);
	BurrowerInitialSpawnDelayPerRound = {5, 5, 5};

	BurrowerSpawnMultiplierPerRoundCurve.SetNum(3);
	
	HunterInitialSpawnDelayPerRound.SetNum(3);
	HunterInitialSpawnDelayPerRound = {10, 10, 10};

	BurstWaveDurationPerRound.SetNum(3);
	BurstWaveDurationPerRound = {10, 10, 10};
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

		BlindEyeGS->FGameTimeSkippedDelegate.AddDynamic(this, &ADifficultyManager::GameTimeSkipped);
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
	BurrowerTimerTimelineFloat.BindDynamic(this, &ADifficultyManager::BurrowerSpawnTimer);
	HunterTimerTimelineFloat.BindDynamic(this, &ADifficultyManager::HunterSpawnTimer);
	
	OnNewRound(0, BlindEyeGS->GetCurrRoundLength());
	PlayHunterSpawnTimeline();
}
 
void ADifficultyManager::OnNewRound(uint8 CurrRound, float roundLength)
{
	StopBurstWave();
	for (FBurrowerSpawningInfo& SpawnInfo : IslandSpawnInfo)
	{
		SpawnInfo.bFirstSpawn = true;
		ResetBurrowerSpawnTimer(SpawnInfo, CurrRound);
		bIsFirstHunterSpawn = true;
		ResetHunterSpawnTimer(CurrRound);
	}
	PlayBurrowerSpawnTimelines(CurrRound, roundLength);

	UWorld* World = GetWorld();
	if (ensure(World))
	{
		World->GetTimerManager().SetTimer(BurstWaveTimerHandle, this, &ADifficultyManager::PerformBurstWave,
			roundLength - BurstWaveDurationPerRound[CurrRound], false);
	}
}

void ADifficultyManager::PlayBurrowerSpawnTimelines(uint8 CurrRound, float roundLength)
{
	// TODO: Destroy Timeline before resetting?
	// Setup new Timeline
	CurrBurrowerSpawnMultTimeline = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass());
	CurrBurrowerSpawnMultTimeline->RegisterComponent();
	CurrBurrowerSpawnMultTimeline->PrimaryComponentTick.bCanEverTick = true;
	CurrBurrowerSpawnMultTimeline->AddInterpFloat(BurrowerSpawnMultiplierPerRoundCurve[CurrRound], BurrowerTimerTimelineFloat);
	
	// Prevent going over 1 second for scaling reason
	CurrBurrowerSpawnMultTimeline->SetTimelineLength(1);
	// Scale spawner playback to match round length
	CurrBurrowerSpawnMultTimeline->SetPlayRate(1 / roundLength);
	CurrBurrowerSpawnMultTimeline->PlayFromStart();
}

void ADifficultyManager::BurrowerSpawnTimer(float Value)
{
	// prevent normal spawning if in burst wave
	if (bInBurstWave) return;
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Green, TEXT("Burrower Spawn Multiplier: ") + FString::SanitizeFloat(Value));
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
				ResetBurrowerSpawnTimer(SpawnInfo, BlindEyeGS->GetCurrRound());
			}
		}
	}
}

void ADifficultyManager::PlayHunterSpawnTimeline()
{
	// TODO: Destroy Timeline before resetting?
	HunterSpawnMultTimeline = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass());
	HunterSpawnMultTimeline->RegisterComponent();
	HunterSpawnMultTimeline->PrimaryComponentTick.bCanEverTick = true;
	HunterSpawnMultTimeline->AddInterpFloat(HunterSpawnMultiplierCurve, HunterTimerTimelineFloat);
	
	// Prevent going over 1 second for scaling reason
	HunterSpawnMultTimeline->SetTimelineLength(1);
	// Scale spawner playback to match round length
	HunterSpawnMultTimeline->SetPlayRate(1 / BlindEyeGS->TimerUntilGameWon);
	HunterSpawnMultTimeline->PlayFromStart();
}

void ADifficultyManager::HunterSpawnTimer(float Value)
{
	// prevent normal spawning if in burst wave
	if (bInBurstWave) return;
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Green,  TEXT("Hunter Spawn Multiplier: ") + FString::SanitizeFloat(Value));

	UWorld* World = GetWorld();
	check(World)

	// Find Hunter controller if not cached yet
	if (HunterController == nullptr)
	{
		HunterController = Cast<AHunterEnemyController>(UGameplayStatics::GetActorOfClass(World, AHunterEnemyController::StaticClass()));
		if (HunterController == nullptr) return;
	}

	// Only decrement timer if no hunter in level
	if (!HunterController->IsHunterSpawned())
	{
		// spawn hunter once timer up
		CurrHunterSpawnTime -= World->GetDeltaSeconds() * Value;
		if (CurrHunterSpawnTime <= 0)
		{
			ResetHunterSpawnTimer(BlindEyeGS->GetCurrRound());
			HunterController->SpawnHunter();
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

void ADifficultyManager::ResetBurrowerSpawnTimer(FBurrowerSpawningInfo& SpawnInfo, uint8 CurrRound)
{ 
	float RandOffsetLengthen = UKismetMathLibrary::RandomFloatInRange(0, BurrowerSpawnVariabilityLengthen);
	float RandOffsetShorten = UKismetMathLibrary::RandomFloatInRange(0, BurrowerSpawnVariabilityShorten);

	if (SpawnInfo.bFirstSpawn) 
	{
		SpawnInfo.bFirstSpawn = false;
		SpawnInfo.RemainingTime = BurrowerInitialSpawnDelayPerRound[CurrRound] + RandOffsetLengthen - RandOffsetShorten;
	}else
	{
		SpawnInfo.RemainingTime = BurrowerBaseSpawnDelayPerRound[CurrRound] + RandOffsetLengthen - RandOffsetShorten;
	}
}

void ADifficultyManager::ResetHunterSpawnTimer(uint8 CurrRound)
{
	if (bIsFirstHunterSpawn)
	{
		bIsFirstHunterSpawn = false;
		float RandOffsetLengthen = UKismetMathLibrary::RandomFloatInRange(0, HunterInitialSpawnVariabilityLengthen);
		float RandOffsetShorten = UKismetMathLibrary::RandomFloatInRange(0, HunterInitialSpawnVariabilityShorten);
		CurrHunterSpawnTime = HunterInitialSpawnDelayPerRound[CurrRound] + RandOffsetLengthen - RandOffsetShorten;
	} else
	{
		CurrHunterSpawnTime = HunterBaseSpawnRate;
	}
}

void ADifficultyManager::GameTimeSkipped(float TimeSkipped)
{
	float CurrRoundTimePercent = BlindEyeGS->GetPercentOfRoundFinished();
	CurrBurrowerSpawnMultTimeline->SetPlaybackPosition(CurrRoundTimePercent, false);
	float CurrGameTimePercent = BlindEyeGS->CurrGameTime / BlindEyeGS->TimerUntilGameWon;
	HunterSpawnMultTimeline->SetPlaybackPosition(CurrGameTimePercent, false);

	UWorld* World = GetWorld();
	if (ensure(World))
	{
		// Update Burst wave timer handle
		float remainingTimeInRound = BlindEyeGS->GetCurrRoundLength() - BlindEyeGS->CurrRoundTimer;
		float NewBurstWaveTimerLength = remainingTimeInRound - BurstWaveDurationPerRound[BlindEyeGS->GetCurrRound()];
		// If skip went into burst wave, start burst wave
		if (NewBurstWaveTimerLength <= 0)
		{
			PerformBurstWave();
			World->GetTimerManager().ClearTimer(BurstWaveTimerHandle);
		}
		// otherwise, update time until burst wave
		else
		{
			World->GetTimerManager().SetTimer(BurstWaveTimerHandle, this, &ADifficultyManager::PerformBurstWave,
			NewBurstWaveTimerLength, false);
		} 
	}
}

void ADifficultyManager::PerformBurstWave()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green,  TEXT("Start Burst Wave"));
	bInBurstWave = true;
	// TODO: Start burst wave
}

void ADifficultyManager::StopBurstWave()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green,  TEXT("End Burst Wave"));
	bInBurstWave = false;
	// TODO: Stop burst wave
}

