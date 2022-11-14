// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameMode.h"

#include "EngineUtils.h"
#include "Characters/BlindEyePlayerController.h"
#include "Characters/PlayerStartingCutscenePosition.h"
#include "Enemies/Burrower/BurrowerSpawnManager.h"
#include "Tutorial/DummyEnemy.h"
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
		//BlindEyeController->SetInputMode(FInputModeUIOnly());
	}

	PlayersConnected++;
	if (PlayersConnected >= 2)
	{
		if (InProgressMatchState == InProgressStates::WaitingLoadingPhase)
		{
			GetWorldTimerManager().SetTimer(StartingTutorialTimerHandle, this, &ABlindEyeGameMode::DelayedStartTutorial, 2, false);
		}
	}
}

void ABlindEyeGameMode::DelayedStartTutorial()
{
	SetInProgressMatchState(InProgressStates::Tutorial);
}

void ABlindEyeGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	IslandManager = Cast<AIslandManager>(UGameplayStatics::GetActorOfClass(world, AIslandManager::StaticClass()));
	check(IslandManager);

	TutorialManager = Cast<ATutorialManager>(UGameplayStatics::GetActorOfClass(world, ATutorialManager::StaticClass()));
	check(TutorialManager)
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
	SetInProgressMatchState(InProgressStates::WaitingLoadingPhase);
}

void ABlindEyeGameMode::TutorialState()
{
	// TODO: Add this logic to when all players have first connected
	TutorialManager->StartTutorials();
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
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)

	// For debugging, if single player waiting, can goto tutorial without 2 players
	if (InProgressMatchState == InProgressStates::WaitingLoadingPhase)
	{
		SetInProgressMatchState(InProgressStates::Tutorial);
		return;
	}

	// trying to skip tutorial while not in tutorial
	if (BlindEyeGS->CurrEnemyTutorial == EEnemyTutorialType::None && !BlindEyeGS->bInBeginningTutorial)
	{
		return;
	}

	// Set Player as finishing tutorial and check if both player's tutorials finished
	uint8 NumPlayersFinishedTutorial = 0;
	for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
	{
		ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(PlayerState);
		if (Player == BlindEyePS->GetPawn())
		{
			BlindEyePS->SetTutorialFinished(true);
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
		// reset tutorial being finished for future tutorial skips
		for (APlayerState* PS : BlindEyeGS->PlayerArray)
		{
			ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(PS);
			BlindEyePS->SetTutorialFinished(false);
		}
	}
}

void ABlindEyeGameMode::OnAllPlayersFinishedTutorial()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	check(BlindEyeGS)

	// If enemy tutorial
	if (BlindEyeGS->CurrEnemyTutorial > EEnemyTutorialType::None)
	{
		if ( BlindEyeGS->bInEnemyTutorialSkippableSection)
		{
			// End Burrower/Snapper enemy tutorial
			if (BlindEyeGS->CurrEnemyTutorial == EEnemyTutorialType::BurrowerSnapper)
			{
				BlindEyeGS->EnemyTutorialTextSkipped();
				StartGame();
			}
			// End Hunter Enemy tutorial
			else if (BlindEyeGS->CurrEnemyTutorial == EEnemyTutorialType::Hunter)
			{
				BlindEyeGS->EnemyTutorialTextSkipped();
			}
		} else
		{
			BlindEyeGS->FinishEnemyTutorial();
			StartGame();
		}
		
	}
	// Otherwise, Beginning tutorial section skipped
	else if (BlindEyeGS->bInBeginningTutorial)
	{
		// Notify players to goto shrine to start game
		// TODO: Notify shrine to wait for all players near to end beginning tutorial
		TutorialManager->SetFinishTutorials();

		// notify tutorial finished, going to first enemy tutorial
		BlindEyeGS->TutorialFinished();
		StartEnemyTutorial(EEnemyTutorialType::BurrowerSnapper);
	}
}

void ABlindEyeGameMode::SpawnTutorialBurrower()
{
	UWorld* World = GetWorld();
	
	// Spawn single burrower from custom tutorial method in SpawnManager
	AActor* SpawnManagerActor = UGameplayStatics::GetActorOfClass(World, ABurrowerSpawnManager::StaticClass());
	ABurrowerSpawnManager* SpawnManager = Cast<ABurrowerSpawnManager>(SpawnManagerActor);

	SpawnManager->TutorialBurrowerSpawn();
}

void ABlindEyeGameMode::StartGame()
{
	SetInProgressMatchState(InProgressStates::GameInProgress);
	
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
    check(BlindEyeGS)
    BlindEyeGS->MULT_StartGame();

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

void ABlindEyeGameMode::OnPlayerDied(ABlindEyePlayerState* DeadPlayer)
{
	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGameState->OnPlayerDied(DeadPlayer);

	if (BlindEyeGameState->DeadPlayers.Num() >= BlindEyeGameState->PlayerArray.Num())
	{
		OnGameEnded();
	}
}

void ABlindEyeGameMode::OnPlayerRevived(ABlindEyePlayerState* RevivedPlayer)
{
	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGameState->OnPlayerRevived(RevivedPlayer);
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
		if (CurrRound < NumRounds)
		{
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

void ABlindEyeGameMode::StartEnemyTutorial(EEnemyTutorialType EnemyTutorial)
{
	switch (EnemyTutorial)
	{
	case EEnemyTutorialType::BurrowerSnapper:
		BurrowerTutorialSetup();
		break;
	case EEnemyTutorialType::Hunter:
		HunterTutorialSetup();
		break;
	case EEnemyTutorialType::None:
		return;
	}
	// Notify BP to play cinematic sequence and continue with enemy tutorial logic
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGS->StartEnemyTutorial(EnemyTutorial);
}

void ABlindEyeGameMode::BurrowerTutorialSetup()
{
	UWorld* World = GetWorld();
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	
	BlindEyeGS->SetPlayerMovementBlocked(true);
	BlindEyeGS->CurrEnemyTutorial = EEnemyTutorialType::BurrowerSnapper;
	
	// Get Tutorial teleport points for burrower/snapper intro cutscene
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStartingCutscenePosition::StaticClass(),OutActors);
	check(OutActors.Num() >= 2);
	uint8 SpawnPointIndex = 0;

	// Apply position to all players
	for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
	{
		APawn* PlayerPawn = PlayerState->GetPawn();
		PlayerPawn->SetActorTransform(OutActors[SpawnPointIndex]->GetTransform());
		SpawnPointIndex++;
	}
}

void ABlindEyeGameMode::HunterTutorialSetup()
{
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGS->CurrEnemyTutorial = EEnemyTutorialType::Hunter;
	BlindEyeGS->SetPlayerMovementBlocked(true);
	// TODO:
}

void ABlindEyeGameMode::PlayLevelSequence(ULevelSequence* SequenceToPlay)
{
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGS->MULT_PlayLevelSequence(SequenceToPlay);
}

void ABlindEyeGameMode::FinishEnemyTutorial()
{
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
	BlindEyeGS->FinishEnemyTutorial();
}
