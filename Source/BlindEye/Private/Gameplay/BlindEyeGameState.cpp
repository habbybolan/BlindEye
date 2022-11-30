// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Shrine.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Characters/PlayerStartingCutscenePosition.h"
#include "Enemies/Burrower/BurrowerSpawnManager.h"
#include "Enemies/Burrower/BurrowerTutorialSpawnPoint.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameInstance.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABlindEyeGameState::ABlindEyeGameState()
{
}

void ABlindEyeGameState::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	GetShrineReference();

	// Cache Island manager
	AActor* IslandManageActor = UGameplayStatics::GetActorOfClass(world, AIslandManager::StaticClass());
	if (IslandManageActor)
	{
		IslandManager = Cast<AIslandManager>(IslandManageActor);
	}

	// Cache Tutorial manager
	AActor* TutorialManagerActor = UGameplayStatics::GetActorOfClass(world, ATutorialManager::StaticClass());
	if (TutorialManagerActor)
	{
		TutorialManager = Cast<ATutorialManager>(TutorialManagerActor);
	}
}

TArray<TWeakObjectPtr<ABlindEyeEnemyBase>> ABlindEyeGameState::GetAllEnemies()
{
	return AllEnemies;
}

void ABlindEyeGameState::SubscribeToEnemy(ABlindEyeEnemyBase* Enemy)
{
	AllEnemies.Add( MakeWeakObjectPtr(Enemy));
	Enemy->HealthComponent->OnDeathDelegate.AddDynamic(this, &ABlindEyeGameState::EnemyDied);
}

void ABlindEyeGameState::OnLevelShift()
{
	LevelShiftDelegate.Broadcast();
}

void ABlindEyeGameState::OnPulse(uint8 currRound, float roundLength)
{
	CurrRound = currRound;
	CurrRoundLength = roundLength;
	PulseDelegate.Broadcast(CurrRound, roundLength);
}

void ABlindEyeGameState::AddReadyPlayerReference(ABlindEyePlayerCharacter* NewReadyPlayer)
{
	BlindEyePlayers.Add(NewReadyPlayer);
	OnRep_PlayerAdded();
}

void ABlindEyeGameState::OnRep_PlayerAdded()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (BlindEyePlayers.Num() >= 2)
		{
			BlindEyePlayers[0]->CLI_NotifyOfOtherPlayerExistance(BlindEyePlayers[1]);
			BlindEyePlayers[1]->CLI_NotifyOfOtherPlayerExistance(BlindEyePlayers[0]);
		}
	}
}

uint8 ABlindEyeGameState::GetCurrRound()
{
	return CurrRound;
}

float ABlindEyeGameState::GetCurrRoundLength()
{
	return CurrRoundLength;
}

float ABlindEyeGameState::GetCurrRoundTime()
{
	return CurrRoundTimer;
}

float ABlindEyeGameState::GetPercentOfRoundFinished()
{
	return CurrRoundTimer / CurrRoundLength;
}

void ABlindEyeGameState::SkipGameTime(float AmountToSkip)
{
	CurrRoundTimer += AmountToSkip;
	CurrGameTime += AmountToSkip;
	FGameTimeSkippedDelegate.Broadcast(AmountToSkip);
}

void ABlindEyeGameState::OnPlayerDied(ABlindEyePlayerState* PlayerThatDied)
{
	DeadPlayers.Add(PlayerThatDied);
}

void ABlindEyeGameState::OnPlayerRevived(ABlindEyePlayerState* PlayerRevived)
{
	for (uint8 i = 0; i < DeadPlayers.Num(); i++)
	{
		if (DeadPlayers[i] == PlayerRevived)
		{
			DeadPlayers.RemoveAt(i);
			return;
		}	
	}
	// Tried to revive an already revived player
	check(true);
}

void ABlindEyeGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

APlayerState* ABlindEyeGameState::GetOtherPlayer(ABlindEyePlayerCharacter* Player)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState != Player->GetPlayerState())
		{
			return PlayerState;
		}
	}
	return nullptr;
}
  
void ABlindEyeGameState::MULT_DisplayTextSnippet_Implementation(EEnemyTutorialType TutorialType)
{
	bInEnemyTutorialSkippableSection = true;
	BP_EnemyTutorialTrigger_CLI(TutorialType);
}

void ABlindEyeGameState::SER_EnemyTutorialTrigger_Implementation(EEnemyTutorialType TutorialType)
{
	if (bInEnemyTutorialSkippableSection) return;
	MULT_DisplayTextSnippet(TutorialType);

	for (APlayerState* PS : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
		Player->CLI_AddEnemyTutorialTextSnippet(TutorialType);
	}
}

void ABlindEyeGameState::SetPlayerMovementBlocked(bool IsMovementBlocked)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(PlayerState))
		{
			BlindEyePS->bActionsBlocked = IsMovementBlocked;
		}
	}
}

void ABlindEyeGameState::StartEnemyTutorial(EEnemyTutorialType TutorialType)
{
	BP_EnemyTutorialStarted_SER(TutorialType);
}

void ABlindEyeGameState::PerformPulse(uint8 PulseIndex)
{
	BP_PerformPulse_SER(PulseIndex);
}

ATutorialManager* ABlindEyeGameState::GetTutorialManager()
{
	return TutorialManager;
}

void ABlindEyeGameState::MULT_StartLoadingScreen_Implementation()
{
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->AddLoadingScreen();
}

void ABlindEyeGameState::TutorialFinished()
{
	bInBeginningTutorial = false;
	TutorialEndedDelegate.Broadcast();
	MULT_BeginningTutorialFinished();
}

void ABlindEyeGameState::MULT_PlayLevelSequence_Implementation(ULevelSequence* SequenceToPlay)
{ 
	UWorld* World = GetWorld();
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bPauseAtEnd = true;
	Settings.bHideHud = true;
	ALevelSequenceActor* OutActor; 
	CurrSequencePlaying = ULevelSequencePlayer::CreateLevelSequencePlayer(World, SequenceToPlay, Settings, OutActor);
	CurrSequencePlaying->Play();

	// Notify player sequence started
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Character);
		Player->LevelSequenceAction(true);
	}
} 

void ABlindEyeGameState::MULT_BeginningTutorialFinished_Implementation()
{
	BP_BeginningTutorialFinished_CLI();
}

void ABlindEyeGameState::EnemyTutorialTextSkipped()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			Player->CLI_RemoveEnemyTutorialTextSnippet();
		}
	}
	BP_EnemyTutorialTextSkipped_SER(CurrEnemyTutorial);
	MULT_EnemyTutorialTextSkipped();
}

void ABlindEyeGameState::MULT_EnemyTutorialTextSkipped_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	// Notify player that sequence stopped
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Character);
		Player->LevelSequenceAction(false);
	}
}

void ABlindEyeGameState::FinishEnemyTutorial()
{
	if (CurrEnemyTutorial == EEnemyTutorialType::BurrowerSnapper)
	{
		MULT_EnemyTutorialFinished();
	} else if (CurrEnemyTutorial == EEnemyTutorialType::Hunter)
	{
		MULT_EnemyTutorialFinished();
	}
	SetPlayerMovementBlocked(false);
	CurrEnemyTutorial = EEnemyTutorialType::None;
	bInEnemyTutorialSkippableSection = false;
}

void ABlindEyeGameState::MULT_EnemyTutorialFinished_Implementation()
{
	if (CurrSequencePlaying)
	{
		CurrSequencePlaying->GoToEndAndStop();
	}
	CurrSequencePlaying = nullptr;
	BP_EnemyTutorialFinished_CLI(CurrEnemyTutorial);
}

void ABlindEyeGameState::EnemyDied(AActor* EnemyActor)
{
	uint8 Index = 0;
	// remove reference to the actor 
	for (TWeakObjectPtr<ABlindEyeEnemyBase> Enemy : AllEnemies)
	{
		if (EnemyActor == Enemy)
		{
			AllEnemies.RemoveAt(Index);
			return;
		}
		Index++;
	}
}

ABlindEyePlayerCharacter* ABlindEyeGameState::GetRandomPlayer()
{
	return Cast<ABlindEyePlayerCharacter>(PlayerArray[UKismetMathLibrary::RandomIntegerInRange(0, PlayerArray.Num() - 1)]->GetPawn());
}

AShrine* ABlindEyeGameState::GetShrine()
{
	// If shrine not found yet, search level for it (for client if value not replicated fast enough)
	if (!Shrine.IsValid())
	{
		GetShrineReference();
	}

	// Get shrine if it exists
	if (Shrine.IsValid())
	{
		return Shrine.Get();
	}
	return nullptr;
}

AIslandManager* ABlindEyeGameState::GetIslandManager()
{
	return IslandManager;
}


void ABlindEyeGameState::SetInProgressMatchState(FName NewInProgressState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		InProgressMatchState = NewInProgressState;

		// Call the onrep to make sure the callbacks happen
		OnRep_InProgressMatchState();
	}
}

bool ABlindEyeGameState::IsBlindEyeMatchNotInProgress()
{
	return InProgressMatchState == InProgressStates::NotInProgress;
}

bool ABlindEyeGameState::IsBlindEyeMatchWaitingPlayers()
{
	return InProgressMatchState == InProgressStates::WaitingLoadingPhase;
}

bool ABlindEyeGameState::IsBlindEyeMatchTutorial()
{
	return InProgressMatchState == InProgressStates::Tutorial;
}

bool ABlindEyeGameState::IsBlindEyeMatchInProgress()
{
	return InProgressMatchState == InProgressStates::GameInProgress;
}

bool ABlindEyeGameState::IsBlindEyeMatchEnding()
{
	return InProgressMatchState == InProgressStates::GameEnding;
}

bool ABlindEyeGameState::IsBlindEyeMatchEnded()
{
	return InProgressMatchState == InProgressStates::GameEnded;
}

void ABlindEyeGameState::MULT_StartGame_Implementation()
{
	GameStartedDelegate.Broadcast();
}

void ABlindEyeGameState::OnRep_InProgressMatchState()
{
	if (InProgressMatchState == InProgressStates::NotInProgress)
	{
		// TODO?
	}
	else if (InProgressMatchState == InProgressStates::WaitingLoadingPhase)
	{
		// TODO:?
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
	} else if (InProgressMatchState == InProgressStates::GameEnded)
	{
		GameEndedState();
	}
}

void ABlindEyeGameState::TutorialState()
{
	bInBeginningTutorial = true;
	TutorialStartedDelegate.Broadcast();
}

void ABlindEyeGameState::GetShrineReference()
{
	UWorld* World = GetWorld();
	AActor* ShrineActor = UGameplayStatics::GetActorOfClass(World, AShrine::StaticClass());
	if (ShrineActor)
	{
		Shrine = MakeWeakObjectPtr(Cast<AShrine>(ShrineActor));
	}
}

void ABlindEyeGameState::SER_TeleportPlayersForBurrowerTutorials_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	// Get Tutorial teleport points for burrower/snapper intro cutscene
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStartingCutscenePosition::StaticClass(),OutActors);
	check(OutActors.Num() >= 2);
	uint8 SpawnPointIndex = 0;

	// Apply position to all players
	for (APlayerState* PlayerState : PlayerArray)
	{
		APawn* PlayerPawn = PlayerState->GetPawn();
		PlayerPawn->SetActorTransform(OutActors[SpawnPointIndex]->GetTransform());
		SpawnPointIndex++;
	}
}

void ABlindEyeGameState::OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType)
{
	// TODO:
}

void ABlindEyeGameState::OnMarkRemoved(AActor* UnmarkedActor, EMarkerType MarkerType)
{
	// TODO:
}

TArray<ABlindEyePlayerCharacter*> ABlindEyeGameState::GetPlayers()
{
	TArray<ABlindEyePlayerCharacter*> Players;
	for (APlayerState* PlayerState : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
		check(Player)
		Players.Add(Player);
	}
	return Players;
}

void ABlindEyeGameState::GameInProgressState()
{
	CurrGameTime = 0;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(MainGameLoopTimer, this, &ABlindEyeGameState::RunMainGameLoopTimers, MainGameLoopDelay, true);
	}
}

void ABlindEyeGameState::GameEndingState()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		BP_GameEnding_SER(GameOverState);
        GameEndingDelegate.Broadcast();
        SetPlayerMovementBlocked(true);
    
        SER_TeleportPlayersForBurrowerTutorials();
	}
}

void ABlindEyeGameState::GameEndedState()
{
	if (UWorld* World = GetWorld())
	{
		if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Character);
			if (GameOverState == EGameOverState::Lost)
			{
				Player->OnGameLost();
			}
			else if (GameOverState == EGameOverState::Won)
			{
				Player->OnGameWon();
			}
		}
	}
}

ABlindEyePlayerCharacter* ABlindEyeGameState::GetPlayer(EPlayerType PlayerType)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
		check(Player)
		if (Player->PlayerType == PlayerType) return Player;
	}
	// PlayerType player not connected
	return nullptr;
}

void ABlindEyeGameState::UpdateMainGameTimer(float GameTimer, float RoundTimer)
{
	CurrGameTime = GameTimer;
	CurrRoundTimer = RoundTimer;
}

void ABlindEyeGameState::RunMainGameLoopTimers()
{
	CurrGameTime += MainGameLoopDelay;
	CurrRoundTimer += MainGameLoopDelay;
}

float ABlindEyeGameState::GetGameDonePercent()
{
	return CurrGameTime / TimerUntilGameWon;
}

void ABlindEyeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyeGameState, bWinConditionPaused)
	DOREPLIFETIME(ABlindEyeGameState, bHunterAlwaysVisible)
	DOREPLIFETIME(ABlindEyeGameState, GameOverState)
	DOREPLIFETIME(ABlindEyeGameState, InProgressMatchState)
	DOREPLIFETIME(ABlindEyeGameState, CurrGameTime)
	DOREPLIFETIME(ABlindEyeGameState, TimerUntilGameWon)
	DOREPLIFETIME(ABlindEyeGameState, AllEnemies)
	DOREPLIFETIME(ABlindEyeGameState, CurrRound)
	DOREPLIFETIME(ABlindEyeGameState, CurrRoundLength)
	DOREPLIFETIME(ABlindEyeGameState, CurrRoundTimer)
	DOREPLIFETIME(ABlindEyeGameState, NumRounds)
	DOREPLIFETIME(ABlindEyeGameState, Shrine)
	DOREPLIFETIME(ABlindEyeGameState, CurrEnemyTutorial)
	DOREPLIFETIME(ABlindEyeGameState, bInBeginningTutorial)
	DOREPLIFETIME(ABlindEyeGameState, bInEnemyTutorialSkippableSection)
	DOREPLIFETIME(ABlindEyeGameState, BlindEyePlayers)
}
