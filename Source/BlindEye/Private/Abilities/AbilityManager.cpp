// Copyright (C) Nicholas Johnson 2022


#include "Abilities/AbilityManager.h"
#include "Abilities/AbilityBase.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAbilityManager::UAbilityManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	UniqueAbilityTypes.SetNum(2);
}
 
void UAbilityManager::UseAbility(EAbilityTypes abilityType, EAbilityInputTypes abilityUsageType, const FVector& MouseLocation, const FRotator& MouseRotation)
{
	// Run on owning client for immediate action
	AAbilityBase* AbilityToUse = GetAbility(abilityType);
	if (GetOwner()->GetLocalRole() != ROLE_Authority && !IsAbilityUnavailable(AbilityToUse))
	{
		UseAbilityHelper(AbilityToUse, abilityUsageType, MouseLocation, MouseRotation);
	}
	
	SER_UseAbility(abilityType, abilityUsageType, MouseLocation, MouseRotation);
}

void UAbilityManager::UseAbilityHelper(AAbilityBase* AbilityToUser, EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	AbilityToUser->UseAbility(abilityUsageType, Location, Rotation);
}

void UAbilityManager::SER_UseAbility_Implementation(EAbilityTypes abilityType, EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	AAbilityBase* AbilityToUse = GetAbility(abilityType);
	if (!IsAbilityUnavailable(AbilityToUse))
	{
		UseAbilityHelper(AbilityToUse, abilityUsageType, Location, Rotation);
	}
}

AAbilityBase* UAbilityManager::GetAbility(EAbilityTypes abilityType)
{
	switch (abilityType)
	{
		case EAbilityTypes::Basic: return BasicAttack;
		case EAbilityTypes::ChargedBasic: return BasicAttack;
		case EAbilityTypes::Unique1: return UniqueAbilities[0];
		case EAbilityTypes::Unique2: return UniqueAbilities[1];
		case EAbilityTypes::Dash: return Dash;
		default: return BasicAttack;
	}
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

float UAbilityManager::IsMovementSlowBlocked()
{
	if (CurrUsedAbility)
	{
		if (CurrUsedAbility->Blockers.IsMovementSlowBlocked)
		{
			return CurrUsedAbility->Blockers.MovementSlowAmount;
		}
	}
	return 1;
}

void UAbilityManager::SetAbilityInUse(AAbilityBase* abilityInUse)
{
	if (CurrUsedAbility == nullptr)
	{
		CurrUsedAbility = abilityInUse;
	}
}

void UAbilityManager::PerformGenericAbilityNotify()
{
	if (CurrUsedAbility != nullptr)
	{
		CurrUsedAbility->GenericAnimNotify();
	}
}

void UAbilityManager::UpdateCooldownUI(EAbilityTypes abilityType, float CurrCooldown, float MaxCooldown)
{
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		BlindEyeCharacter->BP_UpdateCooldownUI(abilityType, CurrCooldown, MaxCooldown);
	}
}

void UAbilityManager::RefreshAllCooldowns(float CooldownRefreshAmount)
{
	for (AAbilityBase* Ability : AllAbilities)
	{
		Ability->RefreshCooldown(CooldownRefreshAmount);
	}
}

bool UAbilityManager::TryCancelCurrentAbility()
{
	if (CurrUsedAbility != nullptr)
	{
		return CurrUsedAbility->TryCancelAbility();
	}
	return true;
}


// Called when the game starts
void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();

	SetupAbilities();
}

void UAbilityManager::SetupAbilities()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters params;
	params.Instigator = Cast<APawn>(GetOwner());
	params.Owner = GetOwner();
	
	// Create Ability actors
	BasicAttack = world->SpawnActor<AAbilityBase>(BasicAttackType, params);
	Dash = world->SpawnActor<AAbilityBase>(DashType, params);
	ChargedBasicAttack = world->SpawnActor<AAbilityBase>(ChargedBasicAttackType, params);
	for (TSubclassOf<AAbilityBase> AbilityType : UniqueAbilityTypes)
	{
		AAbilityBase* UniqueAbility = world->SpawnActor<AAbilityBase>(AbilityType, params);
		UniqueAbilities.Add(UniqueAbility);
		AllAbilities.Add(UniqueAbility);
	}

	// Basic attack delegates
	if (BasicAttack)
	{
		BasicAttack->AbilityEndedDelegate.BindUObject(this, &UAbilityManager::AbilityEnded);
		BasicAttack->AbilityEnteredRunState.BindUObject(this, &UAbilityManager::SetAbilityInUse);
		AllAbilities.Add(BasicAttack);
	}

	// Dash attack delegates
	if (Dash)
	{
		Dash->AbilityEndedDelegate.BindUObject(this, &UAbilityManager::AbilityEnded);
		Dash->AbilityEnteredRunState.BindUObject(this, &UAbilityManager::SetAbilityInUse);
		AllAbilities.Add(Dash);
	}
	
	// Unique abilities delegates
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

bool UAbilityManager::IsAbilityUnavailable(AAbilityBase* AbilityToUse) const
{
	ABlindEyePlayerCharacter* OwningPlayer = Cast<ABlindEyePlayerCharacter>(GetOwner());

	// Try to prevent using ability if hunter marked if possible
	if (OwningPlayer->GetHealthComponent()->GetIsHunterDebuff())
	{
		return true;
	}
	
	// Allow using if same ability currently in use 
	if (CurrUsedAbility == AbilityToUse)
	{
		return false;  
	}

	// If curr ability is basic attack, cancel that attack
	if (CurrUsedAbility == BasicAttack)
	{
		CurrUsedAbility->TryCancelAbility();
	}
	
	if (OwningPlayer->IsActionsBlocked() && CurrUsedAbility == nullptr)
	{
		return true;
	}
	
	return	(AbilityToUse->GetIsOnCooldown() == true) ||
			(CurrUsedAbility != nullptr);
}

void UAbilityManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAbilityManager, BasicAttack);
	DOREPLIFETIME(UAbilityManager, ChargedBasicAttack);
	DOREPLIFETIME(UAbilityManager, UniqueAbilities);
	DOREPLIFETIME(UAbilityManager, Dash);
	DOREPLIFETIME(UAbilityManager, CurrUsedAbility);
}

