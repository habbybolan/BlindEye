// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterTutorialSpawnPoint.h"

AHunterTutorialSpawnPoint::AHunterTutorialSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetArrowColor(FColor::Green);
	ArrowComponent->SetupAttachment(RootComponent);
}

