// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Shrine.h"
#include "Characters/BlindEyePlayerCharacter.h"
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
	// TODO: USE LOGIC FOR LOSE CONDITION OF ALL PLAYERS DYING
	// Notify owning player that player died
	// UWorld* World = GetWorld();
	// if (APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0))
	// {
	// 	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Controller->GetPawn()))
	// 	{
	// 		// Only notify if is other player
	// 		if (Player != PlayerThatDied->GetPawn())
	// 		{
	// 			Player->OnOtherPlayerDied(PlayerThatDied);
	// 		}
	// 	}
	// }
}

void ABlindEyeGameState::OnPlayerRevived(ABlindEyePlayerState* PlayerRevived)
{
	// TODO: USE LOGIC FOR LOSE CONDITION OF ALL PLAYERS DYING
	// Notify owning player that player revived
	// UWorld* World = GetWorld();
	// if (APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0))
	// {
	// 	// Only notify if is other player
	// 	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Controller->GetPawn()))
	// 	{
	// 		// Only notify if is other player
	// 		if (Player != PlayerRevived->GetPawn())
	// 		{
	// 			Player->OnOtherPlayerRevived(PlayerRevived);
	// 		}
	// 	}
	// }
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
	// TODO: Pause game logic
	// TODO: let BP Find all actors in the level and display text snippets for them
	BP_EnemyTutorialTrigger_CLI(TutorialType);
}

void ABlindEyeGameState::EnemyTutorialTrigger(EEnemyTutorialType TutorialType)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (bInEnemyTutorialSkippableSection) return;
		MULT_DisplayTextSnippet(TutorialType);
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

void ABlindEyeGameState::MULT_WaitingToInteractWithShrine_Implementation() 
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Shrine notifies when all players are nearby to end tutorial
		Shrine->StartWaitingForPlayersToInteract();
	}
	BP_WaitingToInteractWithShrine_CLI();
}

void ABlindEyeGameState::TutorialFinished()
{
	bInBeginningTutorial = false;
	TutorialEndedDelegate.Broadcast();
	MULT_BeginningTutorialFinished();
	StartEnemyTutorial(EEnemyTutorialType::BurrowerSnapper);
}

void ABlindEyeGameState::StartEnemyTutorial(EEnemyTutorialType EnemyTutorial)
{
	SetPlayerMovementBlocked(true);
	switch (EnemyTutorial)
	{
	case EEnemyTutorialType::BurrowerSnapper:
		StartBurrowerSnapperTutorial();
	case EEnemyTutorialType::Hunter:
		StartHunterTutorial();
	}
}

void ABlindEyeGameState::StartBurrowerSnapperTutorial()
{
	CurrEnemyTutorial = EEnemyTutorialType::BurrowerSnapper;
	// Spawn single burrower from custom tutorial method in SpawnManager
	UWorld* World = GetWorld(); 
	AActor* SpawnManagerActor = UGameplayStatics::GetActorOfClass(World, ABurrowerSpawnManager::StaticClass());
	ABurrowerSpawnManager* SpawnManager = Cast<ABurrowerSpawnManager>(SpawnManagerActor);

	ABurrowerEnemy* SpawnedBurrower = SpawnManager->TutorialBurrowerSpawn();
	check(SpawnedBurrower)
}

void ABlindEyeGameState::StartHunterTutorial()
{
	// TODO:
}

void ABlindEyeGameState::MULT_BeginningTutorialFinished_Implementation()
{
	AActor* TutorialSpawnPoint = UGameplayStatics::GetActorOfClass(GetWorld(), ABurrowerTutorialSpawnPoint::StaticClass());
	BP_BeginningTutorialFinished_CLI();
	BP_TutorialBurrowerSpawned_CLI(TutorialSpawnPoint->GetTransform());
}

void ABlindEyeGameState::EnemyTutorialFinished()
{
	MULT_EnemyTutorialFinished();
	SetPlayerMovementBlocked(false);
}

void ABlindEyeGameState::MULT_EnemyTutorialFinished_Implementation()
{
	BP_EnemyTutorialFinished_CLI(CurrEnemyTutorial);
	CurrEnemyTutorial = EEnemyTutorialType::None;
	bInEnemyTutorialSkippableSection = false;
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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Blue, "Player Marked");
	// TODO:
}

void ABlindEyeGameState::OnMarkRemoved(AActor* UnmarkedActor, EMarkerType MarkerType)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Blue, "Player Unmarked");
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
