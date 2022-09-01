// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityState.h"
#include "Components/SceneComponent.h"
#include "AbilityBase.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API AAbilityBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AAbilityBase();

	DECLARE_DELEGATE(FAbilityEndedSignature)
	FAbilityEndedSignature AbilityEndedDelegate;

	// Try to cancel the abilities execution
	UFUNCTION()
	virtual void TryCancelAbility();

	void DelayToNextState(float delay);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

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
	bool UseAbility(bool bIsInputInitiated);

	bool bIsRunning = false;
		
};
