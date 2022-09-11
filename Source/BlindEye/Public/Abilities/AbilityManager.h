// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityManager.generated.h"

// represents what type of actions the current ability's state is blocking
UENUM()
enum class EAbilityBlockers
{
	IsBlockMovement = 0,
	IsBlockAbility = 1,
	IsBlockDamageFeedback = 2,
	IsBlockReceiveDamage = 3
};

// Used for easily identifying which ability was used
UENUM()
enum class EAbilityTypes
{
	Basic = 0,
	ChargedBasic = 1,
	Unique1 = 2, 
	Unique2 = 3,
};
 
UENUM()
enum class EAbilityInputTypes
{
	None,
	Pressed,
	Released
};

class AAbilityBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UAbilityManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityManager();

	// Entrance point for using a specific ability and what input called it
	UFUNCTION(Server, Reliable)
	void SER_UsedAbility(EAbilityTypes abilityType, EAbilityInputTypes abilityUsageType);

	bool IsMovementBlocked();
	bool IsAbilityUnavailable();
	bool IsReceiveDamageBlocked();
	bool IsDamageFeedbackBlocked();

	// Called from ability to signal ability in use
	void SetAbilityInUse(AAbilityBase* abilityInUse);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool IsAbilityUnavailable(AAbilityBase* AbilityToUse) const;

	// Ability Types
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAbilityBase> BasicAttackType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAbilityBase> ChargedBasicAttackType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<AAbilityBase>> UniqueAbilityTypes;

	// Created Abilities
	UPROPERTY(Replicated)
	AAbilityBase* BasicAttack;
	UPROPERTY(Replicated)
	AAbilityBase* ChargedBasicAttack;
	UPROPERTY(Replicated)
	TArray<AAbilityBase*> UniqueAbilities;
	UPROPERTY(Replicated)
	AAbilityBase* CurrUsedAbility;

	UFUNCTION()
	void SetupAbilities();

	UFUNCTION()
	void AbilityEnded();
		
};
