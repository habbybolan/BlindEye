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

		FGameModeEvents::GameModePostLoginEvent.AddUFunction(this, "PlayerEnteredTutorial");
	}
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
				if (PlayerState->GetPawn()->GetController())
				{
					if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetPawn()->GetController()))
					{
						InitializePlayerForTutorial(PlayerController);
						PlayerEnteredTutorial(UGameplayStatics::GetGameMode(World), PlayerController);
					}
				}
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
	
	MULT_NotifyNextTutorial(AllTutorials[CurrTutorialIndex]->GetPlayerTutorialArray(EPlayerType::CrowPlayer),
		AllTutorials[CurrTutorialIndex]->GetPlayerTutorialArray(EPlayerType::PhoenixPlayer));
}

void ATutorialManager::MULT_NotifyNextTutorial_Implementation(const TArray<FTutorialInfo>& CrowChecklist, const TArray<FTutorialInfo>& PhoenixChecklist)
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0))
		{
			if (PlayerController->GetPawn())
			{
				ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerController->GetPawn());
				if (Player->IsLocallyControlled())
				{
					NextTutorialStartedDelegate.Broadcast(Player->PlayerType == EPlayerType::CrowPlayer ? CrowChecklist : PhoenixChecklist);
				}
			}
		}
	}
}

void ATutorialManager::PlayerEnteredTutorial(AGameModeBase* GameModeBase, APlayerController* NewPlayer)
{
	if (bTutorialsRunning)
	{
		if (NewPlayer->IsLocalController())
		{
			if (NewPlayer->GetPawn())
			{
				if (NewPlayer->GetPawn()->GetPlayerState())
				{
					AllTutorials[CurrTutorialIndex]->PlayerEnteredTutorial(NewPlayer->GetPawn()->GetPlayerState());
				}
			}
		}
	}
}

void ATutorialManager::InitializePlayerForTutorial(APlayerController* NewPlayer)
{
	if (NewPlayer->GetPawn())
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(NewPlayer->GetPawn());
		Player->StartTutorial(AllTutorials[CurrTutorialIndex]->GetPlayerTutorialArray(Player->PlayerType));
	}
	
}

