// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityManager.h"
#include "Abilities/AbilityBase.h"

// Sets default values for this component's properties
UAbilityManager::UAbilityManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UniqueAbilityTypes.SetNum(2);
	BasicAttack = GetOwner()->CreateDefaultSubobject<AAbilityBase>(TEXT("BasicAttack"));
}

void UAbilityManager::UsedAbility(EAbilityTypes, AbilityUsageTypes)
{
	// TODO: check if ability on cooldown
	// TODO: Check if curr ability, send input to ability
	//	otherwise, set new Curr ability
}

bool UAbilityManager::IsMovementBlocked()
{
	// TODO: Check used ability
	return false;
}

bool UAbilityManager::IsAbilityBlocked()
{
	// TODO: Check used ability
	return false;
}

bool UAbilityManager::IsReceiveDamageBlocked()
{
	// TODO: Check used ability
	return false;
}

bool UAbilityManager::IsDamageFeedbackBlocked()
{
	// TODO: Check used ability
	return false;
}


// Called when the game starts
void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();
}

void UAbilityManager::InitiateAbilities()
{
	// TODO: Create and attach abilities to player
}

void UAbilityManager::AbilityEnded()
{
}

