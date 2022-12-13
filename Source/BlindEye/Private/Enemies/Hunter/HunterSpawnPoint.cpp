// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterSpawnPoint.h"

#include "Components/ArrowComponent.h"

UHunterSpawnPoint::UHunterSpawnPoint()
{
	PrimaryComponentTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	ArrowComponent->ArrowColor = FColor::Red;
	ArrowComponent->SetupAttachment(this);
}


