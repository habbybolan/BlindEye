// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialManager.h"

#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tutorial/TutorialBase.h"

ATutorialManager::ATutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	// Create all tutorial actors 
	for (TSubclassOf<ATutorialBase> Tutorial : TutorialTypes)
	{
		ATutorialBase* tutorial = World->SpawnActor<ATutorialBase>(Tutorial);
		if (tutorial)
		{
			tutorial->TutorialFinishedDelegate.BindDynamic(this, &ATutorialManager::GotoNextTutorial);
			AllTutorials.Add(tutorial);
		}
	}

	FGameModeEvents::GameModePostLoginEvent.AddUFunction(this, "NewPlayerJoined");
}

void ATutorialManager::StartTutorials()
{
	check(AllTutorials.Num() > 0);
	bTutorialsRunning = true;
	StartNextTutorial();

	if (UWorld* World = GetWorld())
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
		{
			if (PlayerState->GetPawn())
			{
				ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
				Player->StartTutorial();
			}
		} 
	}
}

void ATutorialManager::GotoNextTutorial()
{
	if (bTutorialEnded) return;
	CurrTutorialIndex++;
 
	// If finished all tutorials
	if (CurrTutorialIndex < AllTutorials.Num())
	{
		// Setup delay to starting next tutorial
		GetWorldTimerManager().SetTimer(DelayBetweenTutorialsTimerHandle, this, &ATutorialManager::StartNextTutorial, DelayBetweenTutorials, false);
	} else
	{
		ABlindEyeGameMode* BlindEyeGM = Cast<ABlindEyeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		BlindEyeGM->OnAllPlayersFinishedTutorial();
	}
}

void ATutorialManager::SetFinishTutorials()
{
	bTutorialEnded = true;
	if (bTutorialsRunning && CurrTutorialIndex < AllTutorials.Num())
	{
		AllTutorials[CurrTutorialIndex]->EndTutorial();
	}
	bTutorialsRunning = false;
}

TArray<FTutorialInfo> ATutorialManager::GetCurrentTutorialInfo(EPlayerType PlayerType)
{
	if (PlayerType == EPlayerType::CrowPlayer)
	{
		return AllTutorials[CurrTutorialIndex]->CrowTutorialInfo;
	}
	return AllTutorials[CurrTutorialIndex]->PhoenixTutorialInfo;
}

void ATutorialManager::StartNextTutorial()
{
	AllTutorials[CurrTutorialIndex]->SetupTutorial();
	NextTutorialStartedDelegate.Broadcast();
}

void ATutorialManager::NewPlayerJoined(AGameModeBase* GameModeBase, APlayerController* NewPlayer)
{
	if (!bTutorialsRunning) return;
	
	if (UWorld* World = GetWorld())
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(World);
		if (BlindEyeGS->IsBlindEyeMatchTutorial())
		{
			if (NewPlayer->GetPawn())
			{
				ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(NewPlayer->GetPawn());
				Player->CLI_SetupChecklist();
				AllTutorials[CurrTutorialIndex]->OnPlayerConnected(Player);
				Player->StartTutorial();
			}
		}
	}
}

