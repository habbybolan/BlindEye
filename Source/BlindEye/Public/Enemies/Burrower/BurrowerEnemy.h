// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Chaos/PhysicalMaterials.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "BurrowerEnemy.generated.h"

class IBurrowerSpawnManagerListener;
 
class ASnapperEnemy;
class UHealthComponent;
class ABlindEyePlayerCharacter;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABurrowerEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:

	ABurrowerEnemy(const FObjectInitializer& ObjectInitializer);

	// Spawn the burrower at a point, and either attack or spawn snappers depending on action state
	// void SpawnAction(FTransform SpawnLocation);
	//
	// void AttackAction(ABlindEyePlayerCharacter* target);
	
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

	FOnTimelineFloat SurfacingUpdateEvent; 
	FOnTimelineEvent SurfacingFinishedEvent;

	FOnTimelineFloat HideUpdateEvent; 
	FOnTimelineEvent HideFinishedEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* SurfacingTimelineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* HideTimelineComponent;

	DECLARE_DELEGATE(FActionStateFinishedSignature)
	FActionStateFinishedSignature ActionStateFinished;

	DECLARE_DELEGATE(FSurfacingFinishedSignature)
	FSurfacingFinishedSignature SurfacingFinished;

	DECLARE_DELEGATE(FHidingFinishedSignature)
	FHidingFinishedSignature HidingFinished; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackDelayBeforeEmerging = 1.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackTimeAppearingLength = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* WarningParticle;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FollowParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	TSubclassOf<UBaseDamageType> SurfacingDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	TArray<TEnumAsByte<EObjectTypeQuery>> SurfacingObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	float SurfacingRadius = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	float SurfacingDamage = 20.f;

	UPROPERTY(EditDefaultsOnly)
	float DeathDelay = 1.0f;

	// Stored here purely for passing to Controller
	TScriptInterface<IBurrowerSpawnManagerListener> Listener;
	EIslandPosition IslandType; 

	void SpawnMangerSetup(EIslandPosition islandType, TScriptInterface<IBurrowerSpawnManagerListener> listener);
	
	void StartSurfacing();
	void PerformSurfacingDamage();
	void StartHiding();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SetBurrowerState(bool isHidden, bool bFollowing);

	UFUNCTION()
	void SpawnSnappers(); 

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnWarningParticle();
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_DespawnWarningParticle();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartSurfacingHelper();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartHidingHelper(float StartTime);

	void CancelHide();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_CancelHideHelper();

	UPROPERTY(EditDefaultsOnly) 
	UAnimMontage* SurfacingAnimation;

	UPROPERTY(EditDefaultsOnly)  
	UAnimMontage* SpawnSnapperAnimation;

	bool GetIsSurfaced();
	bool GetIsSurfacing();
	bool GetIsHiding();
	bool GetIsHidden();

	UFUNCTION(BlueprintCallable)
	float PlaySpawnSnapperAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PlaySpawnSnapperAnimationHelper();
 
protected:

	virtual void BeginPlay() override;
	
	FTimerHandle DeathTimerHandle;
	void DestroyBurrower();

	virtual void OnDeath(AActor* ActorThatKilled) override;
	
	TMap<uint32, ASnapperEnemy*> SpawnedSnappers;

	bool bIsSurfaced = false;
	bool bIsSurfacing = false;
	bool bIsHiding = false;

	ECollisionChannel CachedCollisionObject;
	
	UFUNCTION()
	void OnSnapperDeath(AActor* SnapperActor);
	
	TArray<FVector> GetSnapperSpawnPoints();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PlaySurfacingAnimation();

	UPROPERTY()
	UNiagaraComponent* SpawnedWarningParticle;
	UPROPERTY()
	UNiagaraComponent* SpawnedFollowParticle;

	// UFUNCTION()
	// void StartAttackAppearance();
	// UFUNCTION()
	// void PerformAttackAppearance();

	// Timeline functions for burrower appearing

	UFUNCTION()
	void TimelineSurfacingMovement(float Value);

	UFUNCTION()
	void TimelineSurfacingFinished();

	// Timeline functions for hiding burrower

	UFUNCTION()
	void TimelineHideMovement(float Value);

	UFUNCTION()
	void TimelineHideFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SurfacingCurve;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* HideCurve;

	FVector CachedMeshRelativeLocation; 

	TWeakObjectPtr<ABlindEyePlayerCharacter> Target;

	// Used in expiring the burrower from following player forever and delay on re-emerging from ground
	FTimerHandle AttackTimerHandle;
	
	void SetSurfacingHiding();
	void SetDisappeared();
	void SetAppeared();
	void SetFollowing();
	
	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnFollowParticle();
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_DespawnFollowParticle();

	FVector GetHidePosition();
	
	virtual void Destroyed() override;
	
	
};
