// Copyright (C) Nicholas Johnson 2022

#include "Tutorial/TutorialBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

ATutorialBase::ATutorialBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATutorialBase::SetupTutorial()
{
	bRunning = true;
	UWorld* World = GetWorld();
	BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
	check(BlindEyeGS)

	for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
	{
		InitializePlayerForTutorial(PlayerState);
	}
}

void ATutorialBase::InitializePlayerForTutorial(APlayerState* PlayerState)
{
	// prevent player for entering tutorial multiple times
	if (!IsPlayerAlreadyInTutorial(PlayerState))
	{
		if (PlayerState->GetPawn())
		{
			PlayerIDAlreadyBinded.Add(PlayerState->GetPlayerId());
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
			PlayerEnteredTutorialHelper(Player);
		}
	}
}

void ATutorialBase::EndTutorialLogic()
{
	// Clear delay end timer if running
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DelayEndTimerHandle);
	}

	// End tutorial logic
	for (APlayerState* PS : BlindEyeGS->PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			Player->TutorialActionBlockers.Reset();
			Player->CLI_DestroyChecklist();
		}
	}
	
	bRunning = false;
	TutorialFinishedDelegate.ExecuteIfBound();
}

void ATutorialBase::EndTutorial()
{
	if (bEnding) return;
	if (UWorld* World = GetWorld())
	{
		bEnding = true;
		World->GetTimerManager().SetTimer(DelayEndTimerHandle, this, &ATutorialBase::EndTutorialLogic, EndTutorialDelay, false);	
	}
}

TArray<FTutorialInfo>& ATutorialBase::GetPlayerTutorialArray(EPlayerType PlayerType)
{
	if (PlayerType == EPlayerType::CrowPlayer) return CrowTutorialInfo;
	else return PhoenixTutorialInfo;
}

bool ATutorialBase::IsPlayerAlreadyInTutorial(APlayerState* Player)
{
	return PlayerIDAlreadyBinded.Contains(Player->GetPlayerId());
}

void ATutorialBase::UpdateChecklistOfPlayer(EPlayerType PlayerType, uint8 ItemID)
{
	if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(PlayerType))
	{
		Player->CLI_UpdateChecklist(ItemID);
	}
}

void ATutorialBase::AddChecklistItem(EPlayerType PlayerType, uint8 ItemID, FString& text, uint8 MaxCount)
{
	FTutorialInfo TutorialInfo = FTutorialInfo();
	TutorialInfo.Init(ItemID, text, MaxCount);
	GetPlayerTutorialArray(PlayerType).Add(TutorialInfo);
}


