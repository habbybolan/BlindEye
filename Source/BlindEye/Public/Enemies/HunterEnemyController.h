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

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	void SetTargetEnemy(AActor* target);

	bool CanBasicAttack();
	bool IsInBasicAttackRange();
	void PerformBasicAttack();
	
protected:
	TWeakObjectPtr<AActor> Target;

	bool IsBasicAttackOnDelay = false;
	FTimerHandle BasicAttackDelayTimerHandle;

	FTimerHandle SpawnDelayTimerHandle;

	UPROPERTY() 
	AHunterEnemy* Hunter;

	virtual void OnPossess(APawn* InPawn) override;
	void SetCanBasicAttack();

	UFUNCTION()
	void SpawnOnDelay(AActor* HunterKilled);
	UFUNCTION()
	void SpawnHunter();
	
};
