// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameMode.h"

#include "EngineUtils.h"
#include "Characters/BlindEyePlayerController.h"
#include "Enemies/Dummy/DummyEnemy.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Gameplay/BlindEyePlayerState.h"
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

void ABlindEyeGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	IslandManager = Cast<AIslandManager>(UGameplayStatics::GetActorOfClass(world, AIslandManager::StaticClass()));
	check(IslandManager);
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

void ABlindEyeGameMode::HandleMatchHasStarted() 
{
	Super::HandleMatchHasStarted();
	SetInProgressMatchState(InProgressStates::Tutorial);
}

void ABlindEyeGameMode::TutorialState()
{
	// TODO:
}

void ABlindEyeGameMode::GameInProgressState()
{
	// TODO:
}

void ABlindEyeGameMode::GameEndingState()
{
	// TODO:
	SetMatchState(MatchState::WaitingPostMatch);
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
}

void ABlindEyeGameMode::SetInProgressMatchState(FName NewInProgressState)
{
	if (InProgressMatchState == NewInProgressState)
	{
		return;
	}
	
	InProgressMatchState = NewInProgressState;

	ABlindEyeGameState* BlindEyeGS = GetGameState<ABlindEyeGameState>();
	if (BlindEyeGS)
	{
		BlindEyeGS->SetInProgressMatchState(NewInProgressState);
	}

	OnBlindEyeMatchStateSet();
}

void ABlindEyeGameMode::OnBlindEyeMatchStateSet()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, InProgressMatchState.ToString());
	if (InProgressMatchState == InProgressStates::NotInProgress)
	{
		// TODO?
	}
	else if (InProgressMatchState == InProgressStates::WaitingLoadingPhase)
	{
		// TODO?
	}
	else if (InProgressMatchState == InProgressStates::Tutorial)
	{
		TutorialState();
	}
	else if (InProgressMatchState == InProgressStates::GameInProgress)
	{
		GameInProgressState();
	}
	else if (InProgressMatchState == InProgressStates::GameEnding)
	{
		GameEndingState();
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
	// Only allow debug incr. time if game in progress
	if (InProgressMatchState == InProgressStates::GameInProgress)
	{
		GameTimer += 60;
		PulseTimer += 60;
		ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
		check(BlindEyeGameState)
		BlindEyeGameState->SkipGameTime(60);
	}
}

void ABlindEyeGameMode::TutorialFinished(ABlindEyePlayerCharacter* Player)
{
	// TODO: Keep track of players that have readied up to see if game should start
	// TODO: De-spawn dummies, send delegate that tutorial ended
	// TODO: Send delegate that game has started
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)

	// Set Player as finishing tutorial and check if both player's tutorials finished
	uint8 NumPlayersFinishedTutorial = 0;
	for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
	{
		ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(PlayerState);
		if (Player == BlindEyePS->GetPawn())
		{
			BlindEyePS->SetTutorialFinished();
			NumPlayersFinishedTutorial++;
		} else if (BlindEyePS->GetIsTutorialFinished())
		{
			NumPlayersFinishedTutorial++;
		}
	}

	// If all connected players finished tutorial, start game
	if (NumPlayersFinishedTutorial == NumPlayers)
	{
		OnAllPlayersFinishedTutorial();
	}
}

void ABlindEyeGameMode::OnAllPlayersFinishedTutorial()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)

	// Kill all tutorial dummy enemies in level
	TArray<AActor*> DummyActors;
	UGameplayStatics::GetAllActorsOfClass(World, ADummyEnemy::StaticClass(), DummyActors);
	for (AActor* DummyActor : DummyActors)
	{
		ADummyEnemy* DummyEnemy = Cast<ADummyEnemy>(DummyActor);
		DummyEnemy->HealthComponent->Kill();
	}

	// Start the game
	BlindEyeGS->TutorialFinished();
	StartGame();
} 

void ABlindEyeGameMode::StartGame()
{
	SetInProgressMatchState(InProgressStates::GameInProgress);
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
    check(BlindEyeGS)
    BlindEyeGS->StartGame();

	// perform initial level shift after tutorial finished
	BP_LevelShift();
	
	UWorld* World = GetWorld();
	if (World)
	{
		// Run main game loop on timer
		World->GetTimerManager().SetTimer(MainGameLoopTimerHandle, this, &ABlindEyeGameMode::RunMainGameLoop, MainGameLoopDelay, true);

		// Initialize Hunter spawning
		AHunterEnemyController* HunterController = World->SpawnActor<AHunterEnemyController>(HunterControllerType);
		HunterController->Initialize();
	}
}

float ABlindEyeGameMode::GetCurrRoundLength()
{
	// TODO: Have array of round lengths if rounds lengths differ
	return TimerUntilGameWon / NumRounds;
}

void ABlindEyeGameMode::RunMainGameLoop()
{
	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);

	// Increment game timer if not paused
	if (!BlindEyeGameState->bWinConditionPaused)
	{
		GameTimer += MainGameLoopDelay;
		PulseTimer += MainGameLoopDelay;
	}

	// Check for pulse events
	if (PulseTimer >= GetCurrRoundLength())
	{
		PulseTimer = 0;
		CurrRound++;
		BP_Pulse(CurrRound);
		BlindEyeGameState->OnPulse(CurrRound, GetCurrRoundLength());
		BP_LevelShift();
		BlindEyeGameState->OnLevelShift();

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

	TimerForUpdatingGameState += MainGameLoopDelay;
	if (TimerForUpdatingGameState >= DelayInUpdatingGameState)
	{
		TimerForUpdatingGameState = 0;
		UpdateGameStateValues();
	}
}

void ABlindEyeGameMode::InitGameState()
{
	Super::InitGameState();
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)
	BlindEyeGS->TimerUntilGameWon = TimerUntilGameWon;
	BlindEyeGS->CurrRoundLength = GetCurrRoundLength();
	BlindEyeGS->NumRounds = NumRounds;
}

void ABlindEyeGameMode::UpdateGameStateValues()
{
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)
	// update calls here to keep clients in sync with GameMode values
	BlindEyeGS->UpdateMainGameTimer(GameTimer, PulseTimer);
}
