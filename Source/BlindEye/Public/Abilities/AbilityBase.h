// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilityState.h"
#include "Components/SceneComponent.h"
#include "AbilityBase.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UAbilityBase : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Holds all ability states, state progression goes linearly down the list starting at index 0
	TArray<AbilityState*> AbilityStates;
	uint8 CurrState = 0;

	bool bOnCooldown = false;
	FTimerHandle CooldownTimerHandle;

	void SetOffCooldown();

	void EndAbility();
	void TryCancelAbility(); 
	void TryNextState();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnPressDown() PURE_VIRTUAL(UAbilityBase::OnPressDown,);
	void OnReleased() PURE_VIRTUAL(UAbilityBase::OnReleased,);
	void OnCancelled() PURE_VIRTUAL(UAbilityBase::OnCancelled,);

	// Input for attempting to cancel the ability
	void AbilityCancelInput();

	// input for attempting to use/trigger ability effects
	void AbilityUseInput();

	bool bIsRunning = false;
		
};
