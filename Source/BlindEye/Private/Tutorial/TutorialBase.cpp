// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialBase.h"

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
}

void ATutorialBase::EndTutorial()
{
	for (APlayerState* PS : BlindEyeGS->PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			check(Player)
			Player->TutorialActionBlockers.Reset();
		}
	}
	
	bRunning = false;
	TutorialFinishedDelegate.ExecuteIfBound();
}


