// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerEnemy.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Interfaces/BurrowerSpawnManagerListener.h"
#include "BurrowerEnemyController.generated.h"

UENUM(BlueprintType)
enum class EBurrowActionState : uint8
{
	Spawning,
	Attacking,
	Patrolling,
	Cancelled,
	BurstWave
};
 
UENUM(BlueprintType) 
enum class EBurrowerVisibilityState : uint8
{
	Hidden,
	Hiding,
	Surfacing,
	Surfaced
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

	void SpawnSnappers();
	EBurrowActionState GetCurrAction();
	// Cache if next action state is either attacking or spawning
	void CalcNewActionState();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();

	UFUNCTION()
	bool IsSurfacing();

	UFUNCTION()
	bool IsSurfaced();

	UFUNCTION() 
	bool IsHidden();  

	UFUNCTION()
	bool IsHiding();

	void StartSurfacing();
	void StartHiding();

	UFUNCTION()
	void StartWarningParticles();
	 
	UFUNCTION()
	void StopWarningParticles();
 
	void NotifyPlayerEnteredIsland(ABlindEyePlayerCharacter* Player);
	void NotifyPlayerLeftIsland(ABlindEyePlayerCharacter* Player);

	void CancelHide();

	TArray<ABlindEyePlayerCharacter*> GetPlayersOnIsland();

	void SetInBurstState();

	
protected:
	FTimerHandle SpawnTimerHandle;
	
	void CacheSpawnPoints();
	
	void AddNewActionState(EBurrowActionState NewAction);

	TArray<EBurrowActionState> CachedPreviousActions;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnDetonated();

	UPROPERTY()
	ABurrowerEnemy* CachedBurrower;

	UPROPERTY()
	TArray<AActor*> SpawnLocation;
	
};
