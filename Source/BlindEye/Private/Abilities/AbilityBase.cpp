// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityBase.h"

// Sets default values for this component's properties
AAbilityBase::AAbilityBase()
{
}


// Called when the game starts
void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();
}


void AAbilityBase::SetOffCooldown()
{
	bOnCooldown = false;
}

void AAbilityBase::TryCancelAbility()
{
	// TODO: Do functionality later
	//		Probably add IsCancellable as base state boolean and extra cancel logic in the state (cancel method?)
}

void AAbilityBase::EndAbilityLogic()
{
	CurrState = 0;
	bIsRunning = false;
	AbilityEndedDelegate.ExecuteIfBound();
}

void AAbilityBase::EndCurrState()
{
	// TODO: End the current state. If no other inner state, call end state
	CurrState++;
	
	// If finished the last inner state, exit the ability
	if (CurrState >= AbilityStates.Num())
	{
		EndAbilityLogic();
	}
}

void AAbilityBase::AbilityCancelInput()
{
	TryCancelAbility(); 
}

bool AAbilityBase::UseAbility(bool bIsInputInitiated)
{
	return AbilityStates[CurrState]->TryEnterState(bIsInputInitiated);
}

