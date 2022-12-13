// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowRushTarget.h"

// Sets default values
ACrowRushTarget::ACrowRushTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

// Called when the game starts or when spawned
void ACrowRushTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

