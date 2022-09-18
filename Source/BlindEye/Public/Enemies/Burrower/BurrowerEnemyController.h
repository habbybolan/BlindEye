// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "BurrowerEnemyController.generated.h"

UENUM(BlueprintType)
enum class EBurrowActionState : uint8
{
	Spawning,
	Attacking
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABurrowerEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	
	EBurrowActionState GetCurrAction();
	// Cache if next action state is either attacking or spawning
	void CalcNewActionState();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	
protected:
	FTimerHandle SpawnTimerHandle;

	void SpawnActionStart();
	void AttackActionStart();
	void CacheSpawnPoints();
	FTransform FindRandSpawnPoint();
	
	void AddNewActionState(EBurrowActionState NewAction);

	ABlindEyePlayerCharacter* GetRandomPlayerForTarget() const;

	UFUNCTION()
	void ActionStateFinished();

	TArray<EBurrowActionState> CachedPreviousActions;

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ABurrowerEnemy* CachedBurrower;

	UPROPERTY()
	TArray<AActor*> SpawnLocation;
	
};
