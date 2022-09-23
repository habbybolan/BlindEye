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
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=1))
	float MovementSlowAmount = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float CostPercentPerSec = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float InitialCostPercent = 5; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* CrowFlurryParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=1))
	float CrowFlurryLerpSpeed = 0.15;

	void StartCrowFlurry();
	void StopCrowFlurry();

protected:
	
	FTimerHandle CrowFlurryTimerHandle;

	// For calculating crow flurry rotation lerping to control rotation
	FTimerHandle CalculateRotationTimerHandle;
	float CalcRotationDelay = 0.05f;

	FTimerHandle RotateFlurryTimerHandle;

	UFUNCTION(NetMulticast, Unreliable)
	void MULT_RotateFlurry(); 
 
	UFUNCTION(BlueprintImplementableEvent)
	void RotateFlurryHelper();

	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator CurrFlurryRotation;

	bool bFlurryActive = false;

	UFUNCTION()
	void PerformCrowFlurry();

	UFUNCTION()
	void CalcFlurryRotation();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnCrowFlurry(FRotator FlurryRotation);
	UFUNCTION(BlueprintImplementableEvent)
	void MULT_SpawnCrowFlurryHelper();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_DestroyCrowFlurry();
	UFUNCTION(blueprintImplementableEvent)
	void MULT_DestroyCrowFlurryHelper(); 

	UPROPERTY()
	UNiagaraComponent* SpawnedCrowFlurryParticle;

	FTimerHandle CrowFlurryParticleDestroyTimerHandle;
	void DestroyParticles();

	virtual void EndAbilityLogic() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
