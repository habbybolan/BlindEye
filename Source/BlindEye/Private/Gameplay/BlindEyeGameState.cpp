// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameMode.h"
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

void ABlindEyeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyeGameState, bWinConditionPaused)
	DOREPLIFETIME(ABlindEyeGameState, bHunterAlwaysVisible)
	DOREPLIFETIME(ABlindEyeGameState, GameOverState)
	DOREPLIFETIME(ABlindEyeGameState, InProgressMatchState)
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
		// TODO:?
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
