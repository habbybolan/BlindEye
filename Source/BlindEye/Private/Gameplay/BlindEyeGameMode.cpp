// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameMode.h"

#include "EngineUtils.h"
#include "Characters/BlindEyePlayerController.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ABlindEyeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	

	if (ABlindEyePlayerController* BlindEyeController = Cast<ABlindEyePlayerController>(NewPlayer))
	{
		BlindEyeController->SER_SpawnPlayer();
		// TODO: Any match logic needed
	}
}

FTransform ABlindEyeGameMode::GetSpawnPoint() const
{
	for (TActorIterator<APlayerStart> playerStart(GetWorld()); playerStart; ++playerStart)
	{
		if (playerStart->PlayerStartTag != "Spawned")
		{
			playerStart->PlayerStartTag = "Spawned";
			return playerStart->GetTransform();
		}
	}
	return FTransform();
}

void ABlindEyeGameMode::OnShrineDeath()
{
	OnGameEnded();
}

void ABlindEyeGameMode::OnGameEnded()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGameState->GameOverState = EGameOverState::Lost;
	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (ABlindEyePlayerController* BlindEyePlayerController = Cast<ABlindEyePlayerController>(PlayerController))
		{
			BlindEyePlayerController->CLI_GameLost();
		}
	}
}

void ABlindEyeGameMode::OnGameWon()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGameState->GameOverState = EGameOverState::Won;
	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (ABlindEyePlayerController* BlindEyePlayerController = Cast<ABlindEyePlayerController>(PlayerController))
		{
			BlindEyePlayerController->CLI_GameWon();
		}
	}
}

void ABlindEyeGameMode::PerformPulse()
{
	ABlindEyeGameState* BlindEyeGameState = GetGameState<ABlindEyeGameState>();
	check(BlindEyeGameState);

	TArray<APlayerState*> Players = BlindEyeGameState->PlayerArray;
	UWorld* World = GetWorld();
	if (World)
	{
		// Kill all Enemies
		TArray<AActor*> EnemyActors;
		UGameplayStatics::GetAllActorsOfClass(World, ABlindEyeEnemyBase::StaticClass(), EnemyActors);
		for (AActor* EnemyActor : EnemyActors)
		{
			ABlindEyeEnemyBase* Enemy = Cast<ABlindEyeEnemyBase>(EnemyActor);
			Enemy->ApplyPulse(BlindEyeGameState->GetRandomPlayer());
		} 
	}
}

void ABlindEyeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);

	// Increment game timer if not paused
	if (!BlindEyeGameState->bWinConditionPaused)
	{
		GameTimer += DeltaSeconds;
	}
	
	// Level shift check
	if (BlindEyeGameState->bHasLevelShifted == false)
	{
		if (GameTimer > TimeUntilLevelShift)
		{
			BP_LevelShift();
			BlindEyeGameState->bHasLevelShifted = true;
		}
	}

	// Check for pulse events
	if (GameTimer >= TimeBetweenPulses * (CurrPulseIndex + 1))
	{
		CurrPulseIndex++;
		BP_Pulse(CurrPulseIndex);

		// Pulse kills all enemies after duration
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(PulseKillDelayTimerHandle, this, &ABlindEyeGameMode::PerformPulse, PulseKillDelay, false);
		}
	}

	// Won condition check
	if (BlindEyeGameState->GameOverState == EGameOverState::InProgress)
	{
		if (GameTimer > TimerUntilGameWon)
		{
			OnGameWon();
		}
	}
}

void ABlindEyeGameMode::RestartGame()
{
	// TODO: Check in GameSession if can restart?
	GetWorld()->ServerTravel("?Restart",false);
}

void ABlindEyeGameMode::PauseWinCondition(bool IsPauseWinCond)
{
	UWorld* World = GetWorld();
	if (!World) return;

	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGameState->bWinConditionPaused = IsPauseWinCond;
}

void ABlindEyeGameMode::IncrementTimeByAMinute()
{
	GameTimer += 60;
}

void ABlindEyeGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	world->SpawnActor(HunterControllerType);
 
	TimeBetweenPulses = TimerUntilGameWon / NumPulses;
}
