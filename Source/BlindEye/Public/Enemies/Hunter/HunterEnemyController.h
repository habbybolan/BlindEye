// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "HunterEnemyController.generated.h"

class UBurrowerTriggerVolume;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialSpawnDelay = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemy> HunterType;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> IslandTriggerObjectType;

	UPROPERTY(EditDefaultsOnly)
	float AfterStunReturnDelay = 10;

	UPROPERTY(EditDefaultsOnly)
	float AfterDeathReturnDelay = 30;
 
	UPROPERTY(EditDefaultsOnly)
	float AfterKillingPlayerDelay = 10;
	
	void SetAlwaysVisible(bool IsAlwaysVisible);

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();

	void PerformChargedJump();
	void PerformBasicAttack(); 

	bool CanChargedJump(AActor* Target);

	bool CanBasicAttack(AActor* Target); 

	// Debugger functionality for spawning a hunter
	//	If a hunter is already alive, then dont do anything
	void DebugSpawnHunter();

	void TrySetVisibility(bool visibility);
	
	UFUNCTION()
	void OnHunterDeath(AActor* HunterKilled);

	void StartChanneling();

	void OnMarkedPlayerDeath();
	
protected:

	float CachedHealth = 0;

	FTimerHandle InitialSpawnDelayTimerHandle;
 
	TMap<EIslandPosition, UBurrowerTriggerVolume*> TriggerVolumes;
	UPROPERTY()
	UBurrowerTriggerVolume* CurrIsland;

	FTimerHandle ReturnDelayTimerHandle;
	
	FTimerHandle InvisDelayTimerHandle;
	void StunInvisDelayFinished();
	void TargetKilledInvisDelayFinished();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetCachedHealth();

	void RemoveHunterHelper();

	UFUNCTION()
	void SetEnteredNewIsland(UPrimitiveComponent* OverlappedActor, AActor* OtherActor);
 
	bool IsInChargedJumpRange(AActor* Target);
	bool IsInBasicAttackRange(AActor* Target);

	bool IsOnSameIslandAsPlayer(AActor* Target);

	virtual void OnStunStart(float StunDuration) override;
	virtual void OnStunEnd() override;

	void DespawnHunter();

	void DelayedReturn(float ReturnDelay);

	UPROPERTY() 
	AHunterEnemy* Hunter;

	virtual void OnPossess(APawn* InPawn) override;

	UBurrowerTriggerVolume* CheckIslandSpawnedOn();
	
	UFUNCTION()
	void SpawnHunter();

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
