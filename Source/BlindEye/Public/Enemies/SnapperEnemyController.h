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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float DistanceToJumpAttack = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float JumpAttackDelay = 3.f;

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	void SetTargetEnemy(AActor* target);

	bool CanJumpAttack();
	bool IsInJumpAttackRange(AActor* Target);
	void PerformJumpAttack();

protected:

	bool IsJumpAttackOnDelay = false;
	FTimerHandle JumpAttackDelayTimerHandle;

	UPROPERTY()
	ASnapperEnemy* Snapper;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanBasicAttack();

	virtual void OnTauntStart(float Duration, AActor* Taunter) override;
	virtual void OnTauntEnd() override;

	virtual void OnStunStart(float StunDuration) override;
	virtual void OnStunEnd() override;
	
};
