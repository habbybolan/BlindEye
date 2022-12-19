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
	virtual bool TryCancelAbility();

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
	
	virtual void AbilityStarted();

	void StartLockRotation(float Duration);

	// Broadcasts a generic anim notify event to be subscribed by child abilities when waiting for anim notify
	void GenericAnimNotify();

	DECLARE_DYNAMIC_DELEGATE(FAnimNotifySignature);
	FAnimNotifySignature AnimNotifyDelegate;

	void RefreshCooldown(float CooldownRefreshAmount);

	void StoreAimData(FVector aimLocation, FRotator aimRotation);

	void AbilityInnerState(uint8 InnerState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsTopdown();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsLocallyControlled();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	// Holds all ability states, state progression goes linearly down the list starting at index 0
	TArray<FAbilityState*> AbilityStates;
	uint8 CurrState = 0;

	UPROPERTY()
	UAbilityManager* OwningAbilityManager;

	UPROPERTY(Replicated)
	bool bOnCooldown = false;
	UPROPERTY(Replicated)
	float CurrCooldown = 0; 
	FTimerHandle CooldownTimerHandle;
	float CooldownTimerDelay = 0.02;

	FVector AimLocation;
	FRotator AimRotation;
	
	void CalculateCooldown();

	UFUNCTION(Client, Unreliable)
	void CLI_UpdateCooldownUI();
	
	UFUNCTION(Client, Reliable) 
	void CLI_CooldownFinished();

	EAbilityTypes AbilityType;

	// Caches if DelayToNextState() will delay to the inner exit state, or next outer state
	bool bDelayToExit = false;
	// Execute the delay to next state timer
	void ExecuteDelayToNextState();
	
	void SetOffCooldown();

	// called when all ability inner states finished or ability cancelled
	virtual void EndAbilityLogic();

	FTimerHandle NextStateDelayTimerHandle;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AbilityInnerState_CLI(uint8 innerStateNum);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AbilityStarted_CLI();

	UFUNCTION(BlueprintImplementableEvent) 
	void BP_AbilityEnded_CLI();

public:

	void SetOnCooldown();

	// called from state to signal state ended
	void EndCurrState();

	// Input for attempting to cancel the ability
	void AbilityCancelInput();

	// input for attempting to use/trigger ability effects
	void UseAbility(EAbilityInputTypes abilityUsageType,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator);

	bool bIsRunning = false;

	UPROPERTY(Replicated)
	FBlockers Blockers;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MULT_AbilityInnerState(uint8 InnerState);
	void AbilityInnerStateHelper(uint8 InnerState);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_AbilityStarted();
	void AbilityStartedHelper();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_AbilityEnded();
	void AbilityEndedHelper();
		
};
