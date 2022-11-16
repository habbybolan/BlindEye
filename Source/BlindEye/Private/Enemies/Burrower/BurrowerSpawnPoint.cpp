// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnPoint.h"

UBurrowerSpawnPoint::UBurrowerSpawnPoint()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	ArrowComponent->ArrowColor = FColor::Blue;
	ArrowComponent->SetupAttachment(this);
}
