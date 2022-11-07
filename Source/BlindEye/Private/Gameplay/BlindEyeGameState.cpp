// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Characters/BlindEyePlayerCharacter.h"
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

	AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
	if (ShrineActor)
	{
		Shrine = MakeWeakObjectPtr(Cast<AShrine>(ShrineActor));
	}
 
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
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState != PlayerThatDied)
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
			Player->OnOtherPlayerDied(PlayerThatDied);
		}
	}
}

void ABlindEyeGameState::OnPlayerRevived(ABlindEyePlayerState* PlayerRevived)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState != PlayerRevived)
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
			Player->OnOtherPlayerRevived(PlayerRevived);
		}
	}
}

void ABlindEyeGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(PlayerState);
	check(BlindEyePS)
	BlindEyePS->PlayerDeathDelegate.AddDynamic(this, &ABlindEyeGameState::OnPlayerDied);
	BlindEyePS->PlayerRevivedDelegate.AddDynamic(this, &ABlindEyeGameState::OnPlayerRevived);
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
	if (Shrine.IsValid())
		return Shrine.Get();
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

void ABlindEyeGameState::TutorialFinished()
{
	TutorialEndedDelegate.Broadcast();
}

void ABlindEyeGameState::StartGame()
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
	TutorialStartedDelegate.Broadcast();
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
}
