// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityBase.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
AAbilityBase::AAbilityBase()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

bool AAbilityBase::GetIsOnCooldown()
{
	return bOnCooldown;
}

// Called when the game starts
void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityBase::SetOnCooldown()
{
	bOnCooldown = true;
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AAbilityBase::SetOffCooldown, Cooldown, false);
}

void AAbilityBase::SetOffCooldown()
{
	// If cooldown removed by outside source
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	bOnCooldown = false;
}

void AAbilityBase::TryCancelAbility()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, "Ability cancelled");
	// TODO: Do functionality later
	//		Probably add IsCancellable as base state boolean and extra cancel logic in the state (cancel method?)

	// Clean up current state
	AbilityStates[CurrState]->CancelState();
	EndAbilityLogic();
}

void AAbilityBase::DelayToNextState(float delay)
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(NextStateDelayTimerHandle);
	world->GetTimerManager().SetTimer(NextStateDelayTimerHandle, this, &AAbilityBase::EndCurrState, delay, false);
}

void AAbilityBase::EndAbilityLogic()
{
	UWorld* world = GetWorld();
	if (world)
	{
		GetWorldTimerManager().ClearTimer(NextStateDelayTimerHandle);
	}
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, "Ability ended");
	CurrState = 0;
	AbilityStates[CurrState]->ResetState();
	bIsRunning = false;
	AbilityEndedDelegate.ExecuteIfBound();
	SetOnCooldown();
}

void AAbilityBase::EndCurrState()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Cyan, "End curr state");
	AbilityStates[CurrState]->ResetState();
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

void AAbilityBase::UseAbility(EAbilityInputTypes abilityUsageType)
{
	AbilityStates[CurrState]->HandleInput(abilityUsageType);
}

void AAbilityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAbilityBase, Blockers);
}

