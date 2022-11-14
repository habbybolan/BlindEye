// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Shrine.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Characters/PlayerStartingCutscenePosition.h"
#include "Enemies/Burrower/BurrowerSpawnManager.h"
#include "Enemies/Burrower/BurrowerTutorialSpawnPoint.h"
#include "GameFramework/PlayerState.h"
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
 
	AActor* IslandManageActor = UGameplayStatics::GetActorOfClass(world, AIslandManager::StaticClass());
	if (IslandManageActor)
	{
		IslandManager = Cast<AIslandManager>(IslandManageActor);
	}
}

TArray<ABlindEyeEnemyBase*> ABlindEyeGameState::GetAllEnemies()
{
	return AllEnemies;
}

void ABlindEyeGameState::SubscribeToEnemy(ABlindEyeEnemyBase* Enemy)
{
	AllEnemies.Add(Enemy);
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

void ABlindEyeGameState::NotifyOtherPlayerOfPlayerExisting(ABlindEyePlayerCharacter* NewPlayer)
{
	for (APlayerState* PS : PlayerArray)
	{
		APlayerState* ANewPS = NewPlayer->GetPlayerState();
		if (PS != ANewPS)
		{
			ABlindEyePlayerCharacter* OtherPlayer = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			if (OtherPlayer)
			{
				OtherPlayer->NotifyOfOtherPlayerExistance(NewPlayer);
			}
		}
	}
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

void ABlindEyeGameState::EnemyTutorialTrigger(EEnemyTutorialType TutorialType)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (bInEnemyTutorialSkippableSection) return;
		MULT_DisplayTextSnippet(TutorialType);
	}

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
	//Settings.bPauseAtEnd = true;
	Settings.bHideHud = true;
	ALevelSequenceActor* OutActor; 
	CurrSequencePlaying = ULevelSequencePlayer::CreateLevelSequencePlayer(World, SequenceToPlay, Settings, OutActor);
	CurrSequencePlaying->Play(); 
} 

void ABlindEyeGameState::MULT_BeginningTutorialFinished_Implementation()
{
	BP_BeginningTutorialFinished_CLI();
}

void ABlindEyeGameState::EnemyTutorialTextSkipped()
{
	for (APlayerState* PS : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
		Player->CLI_RemoveEnemyTutorialTextSnippet();
	}
	
	BP_EnemyTutorialTextSkipped_SER(CurrEnemyTutorial);
}

void ABlindEyeGameState::FinishEnemyTutorial()
{
	if (CurrEnemyTutorial == EEnemyTutorialType::BurrowerSnapper)
	{
		MULT_BurrowerTutorialFinished();
	} else if (CurrEnemyTutorial == EEnemyTutorialType::Hunter)
	{
		// TODO:
	}
	SetPlayerMovementBlocked(false);
	CurrEnemyTutorial = EEnemyTutorialType::None;
	bInEnemyTutorialSkippableSection = false;
}

void ABlindEyeGameState::MULT_BurrowerTutorialFinished_Implementation()
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
	for (ABlindEyeEnemyBase* Enemy : AllEnemies)
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
		// TODO:?
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
}
