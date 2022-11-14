// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialManager.h"

#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tutorial/TutorialBase.h"

ATutorialManager::ATutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

void ATutorialManager::StartTutorials()
{
	check(AllTutorials.Num() > 0);
	bTutorialsRunning = true;
	StartNextTutorial();
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

void ATutorialManager::StartNextTutorial()
{
	AllTutorials[CurrTutorialIndex]->SetupTutorial();
}

