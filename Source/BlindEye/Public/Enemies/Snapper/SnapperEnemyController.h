// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "SnapperEnemyController.generated.h"

class UBehaviorTree;
class ASnapperEnemy;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASnapperEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump Attack")
	float DistanceToJumpAttack = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump Attack")
	float JumpAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float DistanceToBasicAttack = 200.f;
    	  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float BasicAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayUntilAttackShrineAgain = 3.0f;

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	void SetTargetEnemy(AActor* target);

	bool CanJumpAttack(AActor* target); 
	bool IsInJumpAttackRange(AActor* Target);
	void PerformJumpAttack();

	bool CanBasicAttack(AActor* target);
	void PerformBasicAttack();
	bool IsInBasicAttackRange(AActor* Target);

	void DamageTaken(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	// Called from SnapperEnemy on death event
	void OnSnapperDeath();
	
protected:

	bool IsJumpAttackOnDelay = false;
	FTimerHandle JumpAttackDelayTimerHandle;

	bool IsBasicAttackOnDelay = false;
	FTimerHandle BasicAttackDelayTimerHandle;

	FTimerHandle CooldownToAttackShrineTimerHandle;

	void SetShrineAsTarget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY()
	ASnapperEnemy* Snapper;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanJumpAttack();
	void SetCanBasicAttack();

	virtual void OnTauntStart(float Duration, AActor* Taunter) override;
	virtual void OnTauntEnd() override;

	virtual void OnStunStart(float StunDuration) override;
	virtual void OnStunEnd() override;
	
};
