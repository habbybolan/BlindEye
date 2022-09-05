// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "CrowFlurry.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

class BLINDEYE_API UFirstCrowFlurryState : public FAbilityState
{
public:  
	UFirstCrowFlurryState(AAbilityBase* ability);
	virtual bool TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACrowFlurry : public AAbilityBase
{
	GENERATED_BODY()

public:
	ACrowFlurry();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagePerSec = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Width = 400;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Height = 150;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Range = 400;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CostPercentPerSec = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* CrowFlurryParticle;

	void StartCrowFlurry();
	void StopCrowFlurry();

protected:

	FTimerHandle CrowFlurryTimerHandle;

	UFUNCTION()
	void PerformCrowFlurry();

	UPROPERTY()
	UNiagaraComponent* SpawnedCrowFlurryParticle;

	UPROPERTY()
	AActor* ParticleActor;

	FTimerHandle CrowFlurryParticleDestroyTimerHandle;
	void DestroyParticles();
};
