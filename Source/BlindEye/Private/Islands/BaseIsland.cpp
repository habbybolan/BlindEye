// Copyright (C) Nicholas Johnson 2022

#include "Islands/BaseIsland.h"
#include "Enemies/Burrower/BurrowerTriggerVolume.h"

// Sets default values
ABaseIsland::ABaseIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EmptyRoot = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(EmptyRoot);
	IslandTrigger = CreateDefaultSubobject<UBurrowerTriggerVolume>("Trigger Volume");
	IslandTrigger->SetupAttachment(EmptyRoot);
}

// Called when the game starts or when spawned
void ABaseIsland::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseIsland::Initialize(uint8 islandID)
{
	IslandTrigger->IslandID = islandID;
}

