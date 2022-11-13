// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialManager.h"

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
	AllTutorials[0]->SetupTutorial();
}

void ATutorialManager::GotoNextTutorial()
{
	CurrTutorialIndex++;

	// If finished all tutorials
	if (CurrTutorialIndex <= AllTutorials.Num())
	{
		// TODO: Notify GameMode that all the tutorials are finished (Skip)
	} else
	{
		AllTutorials[CurrTutorialIndex]->SetupTutorial();
	}
}

