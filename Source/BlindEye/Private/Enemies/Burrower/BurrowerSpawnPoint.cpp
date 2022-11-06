// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnPoint.h"

UBurrowerSpawnPoint::UBurrowerSpawnPoint()
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	ArrowComponent->SetupAttachment(this);
}
