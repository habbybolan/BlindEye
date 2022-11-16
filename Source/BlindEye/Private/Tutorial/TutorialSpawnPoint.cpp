// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/TutorialSpawnPoint.h"

ATutorialSpawnPoint::ATutorialSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->ArrowColor = FColor::Green;
	ArrowComponent->SetupAttachment(RootComponent);
}

