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

void UAbilityBase::TryCancelAbility()
{
	// TODO: Do functionality later
	//		Probably add IsCancellable as base state boolean and extra cancel logic in the state (cancel method?)
}

void UAbilityBase::EndAbilityLogic()
{
	CurrState = 0;
	bIsRunning = false;
	AbilityEndedDelegate.ExecuteIfBound();
}

void UAbilityBase::EndCurrState()
{
	// TODO: End the current state. If no other inner state, call end state
	CurrState++;
	
	// If finished the last inner state, exit the ability
	if (CurrState >= AbilityStates.Num())
	{
		EndAbilityLogic();
	}
}

// Called every frame
void UAbilityBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAbilityBase::AbilityCancelInput()
{
	TryCancelAbility(); 
}

void UAbilityBase::AbilityUseInput()
{
	AbilityStates[CurrState]->TryEnterState(true);
}

