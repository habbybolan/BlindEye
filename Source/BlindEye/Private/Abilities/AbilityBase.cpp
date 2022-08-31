// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityBase.h"

// Sets default values for this component's properties
UAbilityBase::UAbilityBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAbilityBase::BeginPlay()
{
	Super::BeginPlay();
}


void UAbilityBase::SetOffCooldown()
{
	bOnCooldown = false;
}

// Called every frame
void UAbilityBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

