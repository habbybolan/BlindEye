// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "Components/SceneComponent.h"
#include "AbilityManager.generated.h"

// represents what type of actions the current ability's state is blocking
enum class AbilityBlockers
{
	IsBlockMovement = 0,
	IsBlockAbility = 1,
	IsBlockDamageFeedback = 2,
	IsBlockReceiveDamage = 3
};

// Used for easily identifying which ability was used
enum class AbilityTypes
{
	Basic = 0,
	ChargedBasic = 1,
	Unique1 = 2,
	Unique2 = 3,
};

enum class AbilityUsageTypes
{
	Pressed = 0,
	Released = 1
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UAbilityManager : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityManager();

	// Entrance point for using a specific ability and what input called it
	void UsedAbility(AbilityTypes, AbilityUsageTypes);

	bool IsMovementBlocked();
	bool IsAbilityBlocked();
	bool IsReceiveDamageBlocked();
	bool IsDamageFeedbackBlocked();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UAbilityBase* BasicAttack;

	UPROPERTY()
	UAbilityBase* ChargedBasicAttack;

	UPROPERTY()
	TArray<TSubclassOf<UAbilityBase>> UniqueAbilities;

	UPROPERTY()
	UAbilityBase* CurrUsedAbility;

	// Create the abilities from subclass and attach to player
	void InitiateAbilities();
		
};
