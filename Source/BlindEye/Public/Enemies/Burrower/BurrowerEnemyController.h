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
	Patrolling
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

	void SpawnMangerSetup(EIslandPosition islandType, UBurrowerSpawnManagerListener listener);
 

	void SpawnSnappers();
	EBurrowActionState GetCurrAction();
	// Cache if next action state is either attacking or spawning
	void CalcNewActionState();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();

	UFUNCTION()
	bool IsSurfacing();

	UFUNCTION()
	bool IsHiding();

	void StartSurfacing();
	void StartHiding();
	 
	void SetBurrowerState(bool isHidden, bool bFollowing);

	FTransform FindRandSpawnPoint();

	UFUNCTION()
	void StartWarningParticles();
	 
	UFUNCTION()
	void StopWarningParticles();
 
	void NotifyPlayerEnteredIsland(ABlindEyePlayerCharacter* Player);
	void NotifyPlayerLeftIsland(ABlindEyePlayerCharacter* Player);

	TArray<ABlindEyePlayerCharacter*> GetPlayersOnIsland();
	
protected:
	FTimerHandle SpawnTimerHandle;
	
	void CacheSpawnPoints();
	
	void AddNewActionState(EBurrowActionState NewAction);
	
	UFUNCTION()
	void ActionStateFinished();
	UFUNCTION()
	void SurfacingFinished();
	UFUNCTION()
	void HidingFinished();

	TArray<EBurrowActionState> CachedPreviousActions;

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ABurrowerEnemy* CachedBurrower;

	UPROPERTY()
	TArray<AActor*> SpawnLocation;

	bool bSurfacing = false; // if burrower currently surfacing
	bool bHiding = false;	// if burrower currently hiding
	
};
