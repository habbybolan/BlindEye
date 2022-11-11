// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerTutorialSpawnPoint.h"

ABurrowerTutorialSpawnPoint::ABurrowerTutorialSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetArrowColor(FColor::Green);
}

