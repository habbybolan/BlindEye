// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "HunterEnemy.generated.h"

UENUM(BlueprintType)
enum class EHunterAttacks : uint8
{
	ChargedAttack,
	BasicAttack
};

class UBaseDamageType;
enum class EHunterStates : uint8;

/**
 * 
 */
UCLASS()
class BLINDEYE_API AHunterEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()
	
public:

	AHunterEnemy(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float AttackMaxWalkSpeed = 450;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float StalkingMaxWalkSpeed = 200;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float RunningMaxWalkSpeed = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=10))
	float JumpUpForce = 0.7;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=200000))
	float ForceApplied = 60000; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> JumpAttackDamageType;

	UPROPERTY(EditDefaultsOnly, Category=ChargedAttack)
	float ChargedAttackCooldown = 10.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedAttack)
	float ChargedAttackDuration = 1.0f;
 
	UPROPERTY(EditDefaultsOnly, Category=ChargedAttack, meta=(ClampMin=0, ClampMax=1))
	float MovementSpeedAlteredDuringChargeAttackCooldown = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedAttack) 
	float ChargedAttackLandingDistanceBeforeTarget = 70.f;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack) 
	UAnimMontage* BasicAttackAnimation; 

	UPROPERTY(BlueprintReadWrite)
	bool IsVisible = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadiusToTurnVisible = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpAttackSwingDelay = 0.3f;

	

	void PerformJumpAttack();
	void PerformChargedAttack();
	void ChargedAttackSwing();
	 
	void PerformBasicAttack();

	void TrySetVisibility(bool visibility);

	void UpdateMovementSpeed(EHunterStates NewHunterState);

	virtual void OnDeath(AActor* ActorThatKilled) override;

	bool GetIsChargedAttackOnCooldown();
	
	bool GetIsAttacking();
 
protected:
	
	FTimerHandle JumpAttackSwingDelayTimerHandle;

	bool bAttacking = false;

	float CachedRunningSpeed;

	FTimerHandle BasicAttackTimerHandle;
	void SetBasicAttackFinished();

	UFUNCTION()
	void OnHunterMarkDetonated();

	UFUNCTION() 
	void OnHunterMarkRemoved();

	void UnsubscribeToTargetMarks();

	bool bChargeAttackCooldown = false;
	void SetChargedAttackOffCooldown();
	FTimerHandle ChargedAttackCooldownTimerHandle;

	float CurrTimeOfChargedAttack = 0;	// Keeps track of how long of the jump the charged attack has been performed for
	FVector ChargedAttackTargetLocation;
	FVector ChargedAttackStartLocation; 
	FTimerHandle PerformingChargedAttackTimerHandle;
	void PerformingJumpAttack();

	// Intermediary method to make RPC call to blueprint implementable method
	UFUNCTION(NetMulticast, Reliable)
	void MULT_TurnVisible(bool visibility);
 
	UFUNCTION(BlueprintImplementableEvent)
	void TrySetVisibiltiyHelper(bool visibility);
	
};

