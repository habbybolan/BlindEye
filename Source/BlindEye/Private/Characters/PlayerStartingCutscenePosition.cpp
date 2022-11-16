// Copyright (C) Nicholas Johnson 2022


#include "Characters/PlayerStartingCutscenePosition.h"

APlayerStartingCutscenePosition::APlayerStartingCutscenePosition()
{ 
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->ArrowColor = FColor::Emerald;
	ArrowComponent->SetupAttachment(RootComponent);
}

