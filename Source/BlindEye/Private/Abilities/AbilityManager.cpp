// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityManager.h"

// Sets default values for this component's properties
UAbilityManager::UAbilityManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAbilityManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

