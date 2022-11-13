// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialBase.h"

ATutorialBase::ATutorialBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATutorialBase::SetupTutorial()
{
	bRunning = true;
}

void ATutorialBase::EndTutorial()
{
	bRunning = false;
}


