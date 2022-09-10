// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Chaos/PhysicalMaterials.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "BurrowerEnemy.generated.h"

class ASnapperEnemy;
class UHealthComponent;
class ABlindEyeCharacter;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABurrowerEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:

	ABurrowerEnemy();

	// Spawn the burrower at a point, and either attack or spawn snappers depending on action state
	void SpawnAction(FTransform SpawnLocation);

	void AttackAction(ABlindEyeCharacter* target);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MinSnappersSpawn = 2;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxSnappersSpawn = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnTimeAppearingLength = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxTimerFollowingPlayerInAttack = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesTraceSpawner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASnapperEnemy> SnapperType;

	FOnTimelineFloat SpawnUpdateEvent; 
	FOnTimelineEvent SpawnFinishedEvent;

	FOnTimelineFloat HideUpdateEvent; 
	FOnTimelineEvent HideFinishedEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* SpawnTimelineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* HideTimelineComponent;

	DECLARE_DELEGATE(FActionStateFinishedSignature)
	FActionStateFinishedSignature ActionStateFinished;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackDelayBeforeEmerging = 1.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackTimeAppearingLength = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* WarningParticle;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FollowParticle;
 
protected:

	virtual void BeginPlay() override;
	
	TArray<ASnapperEnemy*> SpawnedSnappers;
	
	TArray<FVector> GetSnapperSpawnPoints();

	UPROPERTY()
	UNiagaraComponent* SpawnedWarningParticle;
	UPROPERTY()
	UNiagaraComponent* SpawnedFollowParticle;

	UFUNCTION()
	void StartAttackAppearance();
	UFUNCTION()
	void PerformAttackAppearance();

	// Timeline functions for burrower appearing

	UFUNCTION()
	void TimelineSpawnMovement();

	UFUNCTION()
	void TimelineSpawnFinished();

	// Timeline functions for hiding burrower

	UFUNCTION()
	void TimelineHideMovement();

	UFUNCTION()
	void TimelineHideFinished();

	UFUNCTION()
	void StartHideLogic();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SpawnCurve;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* HideCurve;

	FTimerHandle HideTimerHandle;
	FVector CachedSpawnLocation;

	FVector CachedBeforeHidePosition;

	TWeakObjectPtr<ABlindEyeCharacter> Target;

	// Used in expiring the burrower from following player forever and delay on re-emerging from ground
	FTimerHandle AttackTimerHandle;
	
	UFUNCTION()
	void SpawnSnappers();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetAppearingHiding();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetDisappeared();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetAppeared();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetFollowing();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnWarningParticle();
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_DespawnWarningParticle();

	
	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnFollowParticle();
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_DespawnFollowParticle(); 
	
	virtual void Destroyed() override;
	
	
};
