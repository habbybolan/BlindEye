// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/LocomotionTutorial/LocomotionTutorial.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"

void ALocomotionTutorial::BeginPlay()
{
	Super::BeginPlay();

	SetupCheckboxes(EPlayerType::CrowPlayer);
	SetupCheckboxes(EPlayerType::PhoenixPlayer);
}

void ALocomotionTutorial::SetupTutorial()
{
	Super::SetupTutorial();

	// setup initial task values for all players
	PlayersFinishedTasks.SetNum(2);
	for (uint8 i = 0; i < 2; i++)
	{
		FFinishedTasks_Locomotion FinishedTasks;
		FinishedTasks.MaxDashCount = DashCount;
		FinishedTasks.MaxJumpCount = JumpCount;
		PlayersFinishedTasks[i] = FinishedTasks;
	}
}

void ALocomotionTutorial::SetupCheckboxes(EPlayerType PlayerType)
{
	FString MoveText = TEXT("Move using WASD");
	AddChecklistItem(PlayerType, 0, MoveText, 0);
	
	FString SpaceText = TEXT("Press <space> to jump");
	AddChecklistItem(PlayerType, 1, SpaceText, JumpCount);
	 
	FString DashText = TEXT("Press <shift> to dash");
	AddChecklistItem(PlayerType, 2, DashText, DashCount);
}

void ALocomotionTutorial::EndTutorial()
{
	Super::EndTutorial();
}

void ALocomotionTutorial::OnPlayerConnected(ABlindEyePlayerCharacter* Player)
{
	Player->TutorialActionsDelegate.AddDynamic(this, &ALocomotionTutorial::OnPlayerAction);
	Player->TutorialActionBlockers.bUnique2blocked = true;
	Player->TutorialActionBlockers.bUnique1Blocked = true;
	Player->TutorialActionBlockers.bBasicAttackBlocked = true;
}

void ALocomotionTutorial::OnPlayerAction(ABlindEyePlayerCharacter* Player, TutorialInputActions::ETutorialInputActions InputActions)
{
	if (!bRunning) return;
	FFinishedTasks_Locomotion& Tasks = PlayersFinishedTasks[(uint8)Player->PlayerType];
	switch(InputActions)
	{
	case TutorialInputActions::Walk:
		Tasks.bMovement = true;
		UpdateChecklistOfPlayer(Player->PlayerType, 0);
		break;
	case TutorialInputActions::Jump:
		Tasks.CurrJumpCount++;
		UpdateChecklistOfPlayer(Player->PlayerType, 1);
		break;
	case TutorialInputActions::Dash:
		Tasks.CurrDashCount++;
		UpdateChecklistOfPlayer(Player->PlayerType, 2);
		break;
	default:
		break;
	}
	CheckAllPlayersFinished();
}

void ALocomotionTutorial::CheckAllPlayersFinished()
{
	// If all players finished their tasks, end tutorial
	for (FFinishedTasks_Locomotion& FinishedTasks : PlayersFinishedTasks)
	{
		if (!FinishedTasks.IsFinished()) return;
	}
	EndTutorial();
}
