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

	SetupHunterSpawnTimeline();
	OnNewRound(0, BlindEyeGS->GetCurrRoundLength());
}
 
void ADifficultyManager::OnNewRound(uint8 CurrRound, float roundLength)
{ 
	for (FBurrowerSpawningInfo& SpawnInfo : IslandSpawnInfo)
	{
		SpawnInfo.bFirstSpawn = true;
		ResetBurrowerSpawnTimer(SpawnInfo, CurrRound);
	}
	PlayBurrowerSpawnTimelines(CurrRound, roundLength);
}

void ADifficultyManager::PlayBurrowerSpawnTimelines(uint8 CurrRound, float roundLength)
{
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

void ADifficultyManager::SetupHunterSpawnTimeline()
{
	HunterSpawnMultTimeline = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass());
	HunterSpawnMultTimeline->RegisterComponent();
	HunterSpawnMultTimeline->PrimaryComponentTick.bCanEverTick = true;
	HunterSpawnMultTimeline->AddInterpFloat(HunterSpawnMultiplierCurve, HunterTimerTimelineFloat);
	
	// Prevent going over 1 second for scaling reason
	HunterSpawnMultTimeline->SetTimelineLength(1);
	// Scale spawner playback to match round length
	HunterSpawnMultTimeline->SetPlayRate(1 / BlindEyeGS->TimerUntilGameWon);
	HunterSpawnMultTimeline->PlayFromStart();

	CurrHunterSpawnTime = HunterBaseSpawnRate;
}

void ADifficultyManager::HunterSpawnTimer(float Value)
{
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
			CurrHunterSpawnTime = HunterBaseSpawnRate;
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

void ADifficultyManager::GameTimeSkipped(float TimeSkipped)
{
	float CurrRoundTime = BlindEyeGS->GetPercentOfRoundFinished();
	CurrBurrowerSpawnMultTimeline->SetPlaybackPosition(CurrRoundTime, false);
}

