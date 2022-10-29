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

	BaseIslandMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseIslandMesh->SetupAttachment(RootComponent);

	Shield = CreateDefaultSubobject<UStaticMeshComponent>("Shield");
	Shield->SetupAttachment(BaseIslandMesh);
	
	IslandTrigger = CreateDefaultSubobject<UBurrowerTriggerVolume>("Trigger Volume");
	IslandTrigger->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseIsland::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseIsland::Initialize(uint8 islandID)
{
	IslandID = islandID;
	IslandTrigger->IslandID = islandID;
}

