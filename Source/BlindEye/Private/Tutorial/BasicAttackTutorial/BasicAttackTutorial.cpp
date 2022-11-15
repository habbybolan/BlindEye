// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/BasicAttackTutorial/BasicAttackTutorial.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"

void ABasicAttackTutorial::SetupTutorial()
{
	Super::SetupTutorial();

	// subscribe to tutorial actions from all players
	for (APlayerState* PS : BlindEyeGS->PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			Player->TutorialActionsDelegate.AddDynamic(this, &ABasicAttackTutorial::OnPlayerAction);
		}
	}

	// setup initial task values for all players
	PlayersFinishedTasks.SetNum(2);
	for (uint8 i = 0; i < 2; i++)
	{
		FFinishedTasks FinishedTasks;
		FinishedTasks.MaxComboCount = ComboCount;
		PlayersFinishedTasks[i] = FinishedTasks;
	}

	SetupCheckboxes(EPlayerType::CrowPlayer);
	SetupCheckboxes(EPlayerType::PhoenixPlayer);
}

void ABasicAttackTutorial::EndTutorial()
{
	Super::EndTutorial();
}

void ABasicAttackTutorial::SetupCheckboxes(EPlayerType PlayerType)
{
	FString MoveText = TEXT("<Left Click> to perform your basic attack");
	AddChecklistItem(PlayerType, 0, MoveText, 0);
	
	FString SpaceText = TEXT("<Left Click> 3 times in a row to perform a combo and do more damage");
	AddChecklistItem(PlayerType, 1, SpaceText, ComboCount);
}

void ABasicAttackTutorial::OnPlayerAction(ABlindEyePlayerCharacter* Player,
	TutorialInputActions::ETutorialInputActions InputActions)
{
	if (!bRunning) return;
	FFinishedTasks& Tasks = PlayersFinishedTasks[(uint8)Player->PlayerType];
	switch(InputActions)
	{
	case TutorialInputActions::BasicAttack:
		Tasks.bBasicAttack = true;
		UpdateChecklistOfPlayer(Player->PlayerType, 0);
		break;
	case TutorialInputActions::BasicAttackCombo:
		Tasks.CurrComboCount++;
		UpdateChecklistOfPlayer(Player->PlayerType, 1);
		break;
	default:
		break;
	}
	CheckAllPlayersFinished();
}

void ABasicAttackTutorial::CheckAllPlayersFinished()
{
	// If all players finished their tasks, end tutorial
	for (FFinishedTasks& FinishedTasks : PlayersFinishedTasks)
	{
		if (!FinishedTasks.IsFinished()) return;
	}
	EndTutorial();
}
