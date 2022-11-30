// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialManager.h"

#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tutorial/TutorialBase.h"

ATutorialManager::ATutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	bNetLoadOnClient = false;
}

void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
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
	}
}

void ATutorialManager::StartTutorials()
{
	check(AllTutorials.Num() > 0);
	bTutorialsRunning = true;

	for (TWeakObjectPtr<ABlindEyePlayerCharacter> Player : SubscribedPlayers)
	{
		if (Player.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ATutorialManager::StartTutorials] Initialized after subscribed %s"), *Player.Get()->GetName());
			InitializePlayerForTutorial(Player.Get());
		}
	}
	AllTutorials[CurrTutorialIndex]->SetupTutorial();
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
	for (TWeakObjectPtr<ABlindEyePlayerCharacter> Player : SubscribedPlayers)
	{
		if (Player.IsValid())
		{
			Player->CLI_OnNewTutorialStarted(AllTutorials[CurrTutorialIndex]->GetPlayerTutorialArray(Player->PlayerType));
		}
	}
}

void ATutorialManager::SubscribePlayerToTUtorial(ABlindEyePlayerCharacter* Player)
{
	SubscribedPlayers.Add(MakeWeakObjectPtr(Player));
	if (bTutorialsRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATutorialManager::StartTutorials] Initialized On subscribed %s"), *Player->GetName());
		InitializePlayerForTutorial(Player);
	}
}

void ATutorialManager::InitializePlayerForTutorial(ABlindEyePlayerCharacter* Player)
{
	Player->StartTutorial(AllTutorials[CurrTutorialIndex]->GetPlayerTutorialArray(Player->PlayerType));
	AllTutorials[CurrTutorialIndex]->InitializePlayerForTutorial(Player->GetPlayerState());
}

