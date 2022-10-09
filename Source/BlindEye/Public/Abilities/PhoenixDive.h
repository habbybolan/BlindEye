// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GroundTarget.h"
#include "Abilities/AbilityBase.h"
#include "PhoenixDive.generated.h"

// Jumping State
class BLINDEYE_API FStartAbilityState : public FAbilityState
{ 
public:
	FStartAbilityState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

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

// Hanging in the air, waiting for input to launch to ground
class BLINDEYE_API FHitGroundState : public FAbilityState
{ 
public:
	FHitGroundState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0))
	float DurationOfUpwardsForce = 1.f;
	 
	UPROPERTY(EditDefaultsOnly)
	float LaunchDownForcePower = 200000;
 
	UPROPERTY(EditDefaultsOnly) 
	float LaunchUpForcePower = 100000;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGroundTarget> GroundTargetType;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY(EditDefaultsOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> EnemyObjectTypes;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = 0, ClampMax = 90))
	float ClampPitchDegrees = 45;
 
	// launches character upwards and sets transition
	void LaunchPlayerUpwards();
	void HangInAir();
	void HangInAirTimer();
	void LaunchToGround();

	// Wait for ability use animation notify to send out flock
	void PlayAbilityAnimation();

	// Recovering after landing and getting up
	void PlayLandingSectionOfAnim();

	UPROPERTY()
	AGroundTarget* GroundTarget;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DiveAbilityAnim;

protected:

	FTimerHandle HangInAirTimerHandle;
	FTimerHandle MaxHangingTimerHandle;
	
	FTimerHandle UpdateGroundTargetPositionTimerHandle;
	void UpdateGroundTargetPosition();

	UFUNCTION(Client, Reliable)
	void CLI_SpawnGroundTarget();

	UFUNCTION(Client, Reliable) 
	void CLI_StopGroundTarget();

	void hangingInAirExpired();

	FRotator CalculateLaunchViewPoint(FVector& ViewportLocation, FRotator& ViewportRotation);

	FVector CalculateDownwardVectorImpulse(FVector TargetPosition, float Angle);

	UFUNCTION()
	void UseAnimNotifyExecuted();
	UFUNCTION()
	void LandingAnimationFinishExecuted();
	
	void EndLaunchUp();
	UFUNCTION()
	void CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void UnsubscribeToGroundCollision();
	virtual void EndAbilityLogic() override;
};
