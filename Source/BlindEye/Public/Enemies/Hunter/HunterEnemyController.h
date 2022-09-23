// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "HunterEnemyController.generated.h"

UENUM(BlueprintType)
enum class EStrafeDirection : uint8
{
	Left,
	Right,
	Backwards
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
	float DistanceToBasicAttack = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float BasicAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnDelay = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemy> HunterType;

	void SetAlwaysVisible(bool IsAlwaysVisible);

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	bool CanBasicAttack();
	void PerformBasicAttack();

	// Debugger functionality for spawning a hunter
	//	If a hunter is already alive, then dont do anything
	void DebugSpawnHunter();

	void TrySetVisibility(bool visibility);
	
protected:

	bool IsBasicAttackOnDelay = false;
	FTimerHandle BasicAttackDelayTimerHandle;

	FTimerHandle SpawnDelayTimerHandle;

	UPROPERTY() 
	AHunterEnemy* Hunter;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanBasicAttack();

	UFUNCTION()
	void OnHunterDeath(AActor* HunterKilled);
	UFUNCTION()
	void SpawnHunter();
};
