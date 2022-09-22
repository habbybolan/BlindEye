// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "PhoenixDive.generated.h"

// Jumping State
class BLINDEYE_API FJumpState : public FAbilityState
{
public:
	FJumpState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};
 
// Extended in air state 
class BLINDEYE_API FInAirState : public FAbilityState
{
public:
	FInAirState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

// Hanging in the air, waiting for input to launch to ground
class BLINDEYE_API FHangingState : public FAbilityState
{
public:
	FHangingState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

class UBaseDamageType;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class BLINDEYE_API APhoenixDive : public AAbilityBase 
{
	GENERATED_BODY()

public:
	APhoenixDive();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Radius = 450;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float CostPercent = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0))
	float MaxTimeHanging = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* CollisionParticle;
 
	// launches character upwards and sets transition
	void LaunchPlayerUpwards();
	void HangInAir();
	void HangInAirTimer();
	void LaunchToGround();
	
	FTimerHandle HangInAirTimerHandle;
	FTimerHandle MaxHangingTimerHandle;

protected:

	void hangingInAirExpired();
	
	void EndLaunchUp();
	UFUNCTION()
	void CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void UnsubscribeToGroundCollision();
	virtual void EndAbilityLogic() override;
};
