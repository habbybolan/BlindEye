// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityState.h"
#include "AbilityBase.generated.h"

USTRUCT()
struct FBlockers
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsMovementBlocked = false;
	UPROPERTY()
	bool IsDamageReceivedBlocked = false;
	UPROPERTY()
	bool IsDamageFeedbackBlocked = false;
	UPROPERTY()
	bool IsOtherAbilitiesBlocked = false;

	UPROPERTY()
	bool IsMovementSlowBlocked = false;
	UPROPERTY()
	float MovementSlowAmount = 1;

	void Reset()
	{
		IsMovementBlocked = false;
		IsDamageReceivedBlocked = false;
		IsDamageFeedbackBlocked = false;
		IsOtherAbilitiesBlocked = false;
		IsMovementSlowBlocked = false;
		MovementSlowAmount = 1;
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

	/**
	 * Delay to either the exit state that's a part of the current state, or delay to the next outer state entirely.
	 * @param delay		Time it takes to delay to either the inner exit state, or increment to outer next state
	 * @param IsDelayToExit	If delay is to the exit inner state. Otherwise, it increments to the next outer state
	 */
	void DelayToNextState(float delay, bool IsDelayToExit = false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Cooldown = 2;

	bool GetIsOnCooldown();

	bool TryConsumeBirdMeter(float BirdMeterAmount);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AbilityStarted();

	UFUNCTION(BlueprintImplementableEvent) 
	void BP_AbilityEnded();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AbilityInnerState(uint8 innerStateNum);

	void StartLockRotation(float Duration);

	// Broadcasts a generic anim notify event to be subscribed by child abilities when waiting for anim notify
	void GenericAnimNotify();

	DECLARE_DELEGATE(FAnimNotifySignature)
	FAnimNotifySignature AnimNotifyDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	// Holds all ability states, state progression goes linearly down the list starting at index 0
	TArray<FAbilityState*> AbilityStates;
	uint8 CurrState = 0;

	bool bOnCooldown = false;
	FTimerHandle CooldownTimerHandle;

	EAbilityTypes AbilityType;

	// Caches if DelayToNextState() will delay to the inner exit state, or next outer state
	bool bDelayToExit = false;
	// Execute the delay to next state timer
	void ExecuteDelayToNextState();

	void SetOnCooldown();
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
