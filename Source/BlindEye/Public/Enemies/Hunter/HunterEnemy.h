// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "HunterEnemy.generated.h"

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

	UPROPERTY(BlueprintReadWrite)
	bool IsVisible = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadiusToTurnVisible = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpAttackSwingDelay = 0.3f;

	void PerformJumpAttack();
	void JumpAttackSwing();

	void TrySetVisibility(bool visibility);

	void UpdateMovementSpeed(EHunterStates NewHunterState);

	virtual void OnDeath(AActor* ActorThatKilled) override;
 
protected:
	
	FTimerHandle JumpAttackSwingDelayTimerHandle;

	// Intermediary method to make RPC call to blueprint implementable method
	UFUNCTION(NetMulticast, Reliable)
	void MULT_TurnVisible(bool visibility);
 
	UFUNCTION(BlueprintImplementableEvent)
	void TrySetVisibiltiyHelper(bool visibility);
	
};
