// Copyright (C) Nicholas Johnson 2022


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::Stun_Implementation()
{
	// TODO:
}

void UHealthComponent::KnockBack_Implementation()
{
	// TODO:
}

void UHealthComponent::Burn_Implementation()
{
	// TODO:
}

