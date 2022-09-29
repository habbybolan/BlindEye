// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "HunterEnemyController.generated.h"

UENUM(BlueprintType)
enum class EStrafeDirection : uint8
{
	None,
	Left,
	Right
};


UENUM(BlueprintType) 
enum class EHunterStates : uint8
{
	Stalking,
	Attacking,
	Running
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API AHunterEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:
	AHunterEnemyController();
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float DistanceToJumpAttack = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float JumpAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnDelay = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemy> HunterType;

	void SetAlwaysVisible(bool IsAlwaysVisible);

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	bool CanJumpAttack(AActor* Target);
	void PerformJumpAttack();

	// Debugger functionality for spawning a hunter
	//	If a hunter is already alive, then dont do anything
	void DebugSpawnHunter();

	void TrySetVisibility(bool visibility);

	void UpdateMovementSpeed(EHunterStates NewHunterState);
	
protected:

	bool IsJumpAttackOnDelay = false;
	FTimerHandle JumpAttackDelayTimerHandle;

	FTimerHandle SpawnDelayTimerHandle;
 
	bool IsInJumpAttackRange(AActor* Target);

	UPROPERTY() 
	AHunterEnemy* Hunter;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanBasicAttack();

	UFUNCTION()
	void OnHunterDeath(AActor* HunterKilled);
	UFUNCTION()
	void SpawnHunter();

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
