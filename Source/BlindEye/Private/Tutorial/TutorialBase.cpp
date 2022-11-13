// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialBase.h"

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
	bRunning = false;
}


