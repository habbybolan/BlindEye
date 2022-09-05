// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "CrowFlurry.generated.h"

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
	TSubclassOf<UBaseDamageType> DamageType;

	void StartCrowFlurry();
	void StopCrowFlurry();

protected:

	FTimerHandle CrowFlurryTimerHandle;

	UFUNCTION()
	void PerformCrowFlurry();
};
