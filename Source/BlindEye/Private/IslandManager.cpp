// Copyright (C) Nicholas Johnson 2022


#include "IslandManager.h"

// Sets default values
AIslandManager::AIslandManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AIslandManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIslandManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

