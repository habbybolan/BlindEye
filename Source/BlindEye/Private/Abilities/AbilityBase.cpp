// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityBase.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Interfaces/AbilityUserInterface.h"
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

bool AAbilityBase::TryConsumeBirdMeter(float BirdMeterAmount)
{
	if (BirdMeterAmount == 0) return true;

	if (GetInstigator() == nullptr) return false;
	if (IAbilityUserInterface* AbilityUserInterface = Cast<IAbilityUserInterface>(GetInstigator()))
	{
		return AbilityUserInterface->TryConsumeBirdMeter(BirdMeterAmount);
	}
	return true;
}

void AAbilityBase::StartLockRotation(float Duration)
{
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyeCharacter->CLI_StartLockRotationToController(Duration);
	}
}

void AAbilityBase::GenericAnimNotify()
{
	AnimNotifyDelegate.ExecuteIfBound();
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
	// TODO: Do functionality later
	//		Probably add IsCancellable as base state boolean and extra cancel logic in the state (cancel method?)

	// Clean up current state
	AbilityStates[CurrState]->CancelState();
	EndAbilityLogic();
}

void AAbilityBase::DelayToNextState(float delay, bool IsDelayToExit)
{
	bDelayToExit = IsDelayToExit;
	if (delay == 0)
	{
		ExecuteDelayToNextState();
		return;
	}
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(NextStateDelayTimerHandle);
	world->GetTimerManager().SetTimer(NextStateDelayTimerHandle, this, &AAbilityBase::ExecuteDelayToNextState, delay, false);
}

void AAbilityBase::ExecuteDelayToNextState()
{
	// Enter exit state
	if (bDelayToExit)
	{
		AbilityStates[CurrState]->ExitState();
	}
	// Enter the next state
	else
	{
		EndCurrState();
	}
}

void AAbilityBase::EndAbilityLogic()
{
	UWorld* world = GetWorld();
	if (world)
	{
		GetWorldTimerManager().ClearTimer(NextStateDelayTimerHandle);
	}
	
	CurrState = 0;
	AbilityStates[CurrState]->ResetState();
	bIsRunning = false;
	AbilityEndedDelegate.ExecuteIfBound();
	BP_AbilityEnded();
	SetOnCooldown();
}

void AAbilityBase::EndCurrState()
{
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

