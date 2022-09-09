// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
#include "Characters/BlindEyeCharacter.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "BurrowerEnemyController.generated.h"

enum class EBurrowActionState
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
	
	
protected:
	FTimerHandle SpawnTimerHandle;

	void SpawnActionStart();
	void AttackActionStart();
	void CacheSpawnPoints();
	FTransform FindRandSpawnPoint();

	void AddNewActionState(EBurrowActionState NewAction);

	ABlindEyeCharacter* GetRandomPlayerForTarget() const;

	UFUNCTION()
	void ActionStateFinished();

	TArray<EBurrowActionState> CachedPreviousActions;

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ABurrowerEnemy* CachedBurrower;

	UPROPERTY()
	TArray<AActor*> SpawnLocation;
	
};
