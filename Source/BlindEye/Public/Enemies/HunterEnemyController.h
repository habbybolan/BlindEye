// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "HunterEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API AHunterEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float DistanceToBasicAttack = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float BasicAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnDelay = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemy> HunterType;

	void SetAlwaysVisible(bool IsAlwaysVisible);
	bool GetAlwaysVisible();

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	void SetTargetEnemy(AActor* target);

	bool CanBasicAttack();
	bool IsInBasicAttackRange();
	void PerformBasicAttack();

	// Debugger functionality for spawning a hunter
	//	If a hunter is already alive, then dont do anything
	void DebugSpawnHunter();
	
protected:
	TWeakObjectPtr<AActor> Target;

	bool IsBasicAttackOnDelay = false;
	FTimerHandle BasicAttackDelayTimerHandle;

	FTimerHandle SpawnDelayTimerHandle;

	UPROPERTY() 
	AHunterEnemy* Hunter;

	UPROPERTY()
	bool DebugAlwaysVisible = false;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanBasicAttack();

	UFUNCTION()
	void OnHunterDeath(AActor* HunterKilled);
	UFUNCTION()
	void SpawnHunter();
	
};
