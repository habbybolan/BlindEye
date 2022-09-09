// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "CrowCocoon.generated.h"

class BLINDEYE_API UCrowCocoonStart : public FAbilityState
{ 
public:  
	UCrowCocoonStart(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

class BLINDEYE_API UCrowPulseState : public FAbilityState
{ 
public:  
	UCrowPulseState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

class UNiagaraComponent;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACrowCocoon : public AAbilityBase
{
	GENERATED_BODY()

public:

	ACrowCocoon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick1 = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick2 = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick3 = 35;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageTick4 = 50;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BirdMeterPercentLossPerSec = 20; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHoldDuration = 4.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayFirstPulse = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UNiagaraSystem*> PulseParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FullyChargedParticle;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* ExplosionPulse;

	void StartHoldLogic();
	void EndHold();
	virtual void EndAbilityLogic() override;
	
	
protected:

	FTimerHandle PulseTimerHandle;
	float TimeHoldStart;
	const uint8 MaxNumberPulses = 4;
 
	UPROPERTY()
	TArray<UNiagaraComponent*> SpawnedPulseParticles;

	UPROPERTY()
	UNiagaraComponent* SpawnedFullyChargedParticle;
 
	UPROPERTY()
	UNiagaraComponent* SpawnedExplosionPulse;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformPulse();

	UFUNCTION(NetMulticast, Reliable) 
	void MULT_PerformExplosionPulse();
	
};
