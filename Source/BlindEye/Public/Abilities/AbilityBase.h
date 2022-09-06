// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityState.h"
#include "AbilityBase.generated.h"

USTRUCT()
struct FBlockers
{
	GENERATED_BODY()
	
	bool IsMovementBlocked = false;
	bool IsDamageReceivedBlocked = false;
	bool IsDamageFeedbackBlocked = false;
	bool IsOtherAbilitiesBlocked = false;

	void Reset()
	{
		IsMovementBlocked = false;
		IsDamageReceivedBlocked = false;
		IsDamageFeedbackBlocked = false;
		IsOtherAbilitiesBlocked = false;
	}
};


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API AAbilityBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AAbilityBase();

	DECLARE_DELEGATE(FAbilityEndedSignature)
	FAbilityEndedSignature AbilityEndedDelegate;

	DECLARE_DELEGATE_OneParam(FAbilityEnteredRunStateSignature, AAbilityBase* AbilityUsed)
	FAbilityEnteredRunStateSignature AbilityEnteredRunState; 

	// Try to cancel the abilities execution
	UFUNCTION()
	virtual void TryCancelAbility();

	void DelayToNextState(float delay);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Cooldown = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialCostPercent = 5;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	// Holds all ability states, state progression goes linearly down the list starting at index 0
	TArray<FAbilityState*> AbilityStates;
	uint8 CurrState = 0;

	bool bOnCooldown = false;
	FTimerHandle CooldownTimerHandle;

	void SetOffCooldown();

	// called when all ability inner states finished or ability cancelled
	virtual void EndAbilityLogic();

	FTimerHandle NextStateDelayTimerHandle;

public:

	// called from state to signal state ended
	void EndCurrState();

	// Input for attempting to cancel the ability
	void AbilityCancelInput();

	// input for attempting to use/trigger ability effects
	void UseAbility(EAbilityInputTypes abilityUsageType);

	bool bIsRunning = false;

	UPROPERTY(Replicated)
	FBlockers Blockers;
		
};
