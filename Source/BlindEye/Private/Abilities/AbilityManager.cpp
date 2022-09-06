// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityManager.h"
#include "Abilities/AbilityBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAbilityManager::UAbilityManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	UniqueAbilityTypes.SetNum(2);
}
 
void UAbilityManager::SER_UsedAbility_Implementation(EAbilityTypes abilityType, EAbilityInputTypes abilityUsageType)
{
	// prevent using ability if another one activated
	// TODO: Check if ability being used is blocking. If not, cancel ability 
	

	if (abilityType == EAbilityTypes::Basic)
	{
		if (IsAbilityBlocked(BasicAttack)) return;
		BasicAttack->UseAbility(abilityUsageType);
	} else if (abilityType == EAbilityTypes::Unique1)
	{
		if (UniqueAbilities[0])
		{
			if (IsAbilityBlocked(UniqueAbilities[0])) return;
			UniqueAbilities[0]->UseAbility(abilityUsageType);
		}
			
	}
	// TODO: Rest of abilities
}

bool UAbilityManager::IsMovementBlocked()
{
	if (CurrUsedAbility)
	{
		return CurrUsedAbility->Blockers.IsMovementBlocked;
	}
	return false;
}

bool UAbilityManager::IsAbilityBlocked()
{
	if (CurrUsedAbility)
	{
		return CurrUsedAbility->Blockers.IsOtherAbilitiesBlocked;
	}
	return false;
}

bool UAbilityManager::IsReceiveDamageBlocked()
{
	if (CurrUsedAbility)
	{
		return CurrUsedAbility->Blockers.IsDamageReceivedBlocked;
	}
	return false;
}

bool UAbilityManager::IsDamageFeedbackBlocked()
{
	if (CurrUsedAbility)
	{
		return CurrUsedAbility->Blockers.IsDamageFeedbackBlocked;
	}
	return false;
}

void UAbilityManager::SetAbilityInUse(AAbilityBase* abilityInUse)
{
	if (CurrUsedAbility == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, "Ability in use: " + abilityInUse->GetName());
		CurrUsedAbility = abilityInUse;
	}
}


// Called when the game starts
void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();

	SetupAbilities();
}

void UAbilityManager::SetupAbilities()
{
	if (GetOwnerRole() < ROLE_Authority) return;

	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.Instigator = Cast<APawn>(GetOwner());
	params.Owner = GetOwner();
	
	// Create Ability actors
	BasicAttack = world->SpawnActor<AAbilityBase>(BasicAttackType, params);
	ChargedBasicAttack = world->SpawnActor<AAbilityBase>(ChargedBasicAttackType, params);
	for (TSubclassOf<AAbilityBase> AbilityType : UniqueAbilityTypes)
	{
		UniqueAbilities.Add(world->SpawnActor<AAbilityBase>(AbilityType, params));
	}

	if (BasicAttack)
	{
		BasicAttack->AbilityEndedDelegate.BindUObject(this, &UAbilityManager::AbilityEnded);
		BasicAttack->AbilityEnteredRunState.BindUObject(this, &UAbilityManager::SetAbilityInUse);
	}
	
	// TODO: Setup delegates for rest of abilities
	for (AAbilityBase* uniqueAbility : UniqueAbilities)
	{
		if (!uniqueAbility) continue;
		uniqueAbility->AbilityEndedDelegate.BindUObject(this, &UAbilityManager::AbilityEnded);
		uniqueAbility->AbilityEnteredRunState.BindUObject(this, &UAbilityManager::SetAbilityInUse);
	}
}

void UAbilityManager::AbilityEnded()
{
	CurrUsedAbility = nullptr;
}

bool UAbilityManager::IsAbilityBlocked(AAbilityBase* AbilityToUse)
{
	return CurrUsedAbility != nullptr && CurrUsedAbility->Blockers.IsOtherAbilitiesBlocked && CurrUsedAbility != AbilityToUse;
}

void UAbilityManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAbilityManager, BasicAttack);
	DOREPLIFETIME(UAbilityManager, ChargedBasicAttack);
	DOREPLIFETIME(UAbilityManager, UniqueAbilities);
	DOREPLIFETIME(UAbilityManager, CurrUsedAbility);
}

