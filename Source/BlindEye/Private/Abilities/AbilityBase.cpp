// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityBase.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Interfaces/AbilityUserInterface.h"
#include "Kismet/KismetMathLibrary.h"
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

void AAbilityBase::AbilityStarted()
{
	BP_AbilityStarted();
	bIsRunning = true;
}

void AAbilityBase::StartLockRotation(float Duration)
{
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyeCharacter->MULT_StartLockRotationToController(Duration);
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
	CurrCooldown = Cooldown;
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AAbilityBase::CalculateCooldown, CooldownTimerDelay, true);
}

void AAbilityBase::CalculateCooldown()
{
	CurrCooldown -= CooldownTimerDelay;
	// TODO: Lots of network calls
	CLI_UpdateCooldownUI();
	if (CurrCooldown <= 0)
	{
		SetOffCooldown();
	}
}

void AAbilityBase::CLI_CooldownFinished_Implementation()
{
	if (OwningAbilityManager)
	{
		OwningAbilityManager->UpdateCooldownUI(AbilityType, 0, Cooldown);
	}
}

void AAbilityBase::CLI_UpdateCooldownUI_Implementation()
{
	if (OwningAbilityManager == nullptr)
	{
		UActorComponent* AbilityManagerComp = GetOwner()->GetComponentByClass(UAbilityManager::StaticClass());
		if (AbilityManagerComp)
		{
			OwningAbilityManager = Cast<UAbilityManager>(AbilityManagerComp);
		}
	}
	
	if (OwningAbilityManager)
	{
		OwningAbilityManager->UpdateCooldownUI(AbilityType, CurrCooldown, Cooldown);
	}
}

void AAbilityBase::RefreshCooldown(float CooldownRefreshAmount)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	if (bOnCooldown)
	{
		CurrCooldown = UKismetMathLibrary::Max(0, CurrCooldown - CooldownRefreshAmount);
	}
}


void AAbilityBase::SetOffCooldown()
{
	CLI_CooldownFinished();
	// If cooldown removed by outside source
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	bOnCooldown = false;
}

bool AAbilityBase::TryCancelAbility()
{
	// TODO: Do functionality later
	//		Probably add IsCancellable as base state boolean and extra cancel logic in the state (cancel method?)

	// Clean up current state
	if (AbilityStates[CurrState]->CancelState())
	{
		EndAbilityLogic();
		return true;
	}
	return false;
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

	if (!bOnCooldown)
	{
		SetOnCooldown();
	}
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, "Ability ended");
	CurrState = 0;
	AbilityStates[CurrState]->ResetState();
	bIsRunning = false;
	AbilityEndedDelegate.ExecuteIfBound();
	BP_AbilityEnded();
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
	if (AbilityStates.Num() <= CurrState) return;
	AbilityStates[CurrState]->HandleInput(abilityUsageType);
}

void AAbilityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAbilityBase, Blockers);
	DOREPLIFETIME(AAbilityBase, CurrCooldown);
	DOREPLIFETIME(AAbilityBase, bOnCooldown);
}

