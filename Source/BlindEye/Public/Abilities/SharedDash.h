// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "SharedDash.generated.h"

// Dash State
class BLINDEYE_API FDashStartState : public FAbilityState
{
public:
	FDashStartState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None,
		const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator) override;
	virtual void ExitState() override;
	virtual bool CancelState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASharedDash : public AAbilityBase
{
	GENERATED_BODY()

public:
	ASharedDash();

	UPROPERTY(EditDefaultsOnly)
	float DashDuration = 0.8f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	float DashSpeedIncrease = 1.5f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	float DashAccelerationIncrease = 1.5f;

	virtual void AbilityStarted() override;

	void UpdatePlayerSpeed(); 
	void ResetPlayerSpeed();

	virtual void EndAbilityLogic() override;
	
};
