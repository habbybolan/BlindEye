// Copyright (C) Nicholas Johnson 2022


#include "Enemies/DifficultyManager.h"

// Sets default values
ADifficultyManager::ADifficultyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADifficultyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADifficultyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

