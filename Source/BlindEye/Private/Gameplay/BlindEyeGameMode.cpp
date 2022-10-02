// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameMode.h"

#include "EngineUtils.h"
#include "Characters/BlindEyePlayerController.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/BlindEyeGameState.h"

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

void ABlindEyeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);

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

}
