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
		if (PlayerState->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
			Player->CLI_SetupChecklist();
		}
	}
}

void ATutorialBase::EndTutorial()
{
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

void ATutorialBase::UpdateChecklistOfPlayer(EPlayerType PlayerType, uint8 ItemID)
{
	if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(PlayerType))
	{
		Player->CLI_UpdateChecklist(ItemID);
	}
}

void ATutorialBase::AddChecklistItem(EPlayerType PlayerType, uint8 ItemID, FString& text, uint8 MaxCount)
{
	if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(PlayerType))
	{
		Player->CLI_AddChecklist(ItemID, text, MaxCount);
	}
}


