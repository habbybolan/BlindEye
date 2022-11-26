// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BurrowerSpawnPoint.h"
#include "NiagaraComponent.h"
#include "Chaos/PhysicalMaterials.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Islands/Island.h"
#include "BurrowerEnemy.generated.h"

enum class EBurrowerVisibilityState : uint8;

class IBurrowerSpawnManagerListener;  
class ASnapperEnemy;
class UHealthComponent;
class ABlindEyePlayerCharacter;
class UBaseDamageType;

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
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Spawning)
	float ForwardSnapperSpawnForce = 30000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Spawning)
	float SnapperSpawnAngleToLeft = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category=Spawning, meta=(ToolTip="Variability in time of snappers getting up froim ragdoll after burrower finished spawning group"))
	float SnapperRagdollTimeVariabilityAfterGroupSpawned = 0.2f;
     
    UPROPERTY(EditDefaultsOnly, Category=Spawning, meta=(ToolTip="Base time for snappers to stay ragdolled after finished spawning group"))
    float SnapperRagdollBaseTimeAfterGroupSpawned = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnTimeAppearingLength = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxTimerFollowingPlayerInAttack = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesTraceSpawner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASnapperEnemy> SnapperType;

	UPROPERTY()
	FOnTimelineFloat SurfacingUpdateEvent;
	UPROPERTY()
	FOnTimelineEvent SurfacingFinishedEvent;

	UPROPERTY()
	FOnTimelineFloat HideUpdateEvent;
	UPROPERTY()
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	TSubclassOf<UBaseDamageType> SurfacingDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	TArray<TEnumAsByte<EObjectTypeQuery>> SurfacingObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	float SurfacingRadius = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SurfacingDamage)
	float SurfacingDamage = 20.f;

	// Stored here purely for passing to Controller
	TScriptInterface<IBurrowerSpawnManagerListener> Listener;
	uint8 IslandID;

	bool bIsTutorialBurrower = false;

	void SpawnMangerSetup(uint8 islandID, TScriptInterface<IBurrowerSpawnManagerListener> listener);
	
	void PerformSurfacingDamage();

	UFUNCTION()
	void SpawnSnappers(); 

	void CancelHide();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_CancelHideHelper();

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

	UFUNCTION(NetMulticast, Unreliable)
	void MULT_WarningStarted();
	UFUNCTION(NetMulticast, Unreliable)
	void MULT_WarningEnded();

	UFUNCTION(BlueprintCallable)
	EBurrowerVisibilityState GetVisibilityState();

	void SubscribeToSpawnLocation(UBurrowerSpawnPoint* SpawnPoint);
	void UnsubscribeToSpawnLocation();

	void SubscribeToIsland(AIsland* Island);
	UBurrowerSpawnPoint* GetRandUnusedSpawnPoint();

	void NotifySpawningStopped();

	UFUNCTION()
	void UpdateBurrowerState(EBurrowerVisibilityState NewState);
 
protected:

	virtual void BeginPlay() override;

	virtual void OnDeath(AActor* ActorThatKilled) override;

	UPROPERTY()
	UBurrowerSpawnPoint* CurrUsedSpawnPoint = nullptr;

	UPROPERTY()
	AIsland* OwningIsland = nullptr;

	// bool bIsSurfaced = false;
	// bool bIsSurfacing = false;
	// bool bIsHiding = false;
	// bool bIsHidden = false;
 
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_VisibilityState")
	EBurrowerVisibilityState VisibilityState;

	// Used for client to keep track of its own visibility state if too out of sync with server
	// Updated from timeline methods
	UPROPERTY()
	EBurrowerVisibilityState LocalVisibilityState;
 
	UFUNCTION()
	void OnRep_VisibilityState(EBurrowerVisibilityState OldVisibilityState);
	
	bool bIsFollowing = false;

	ECollisionChannel CachedCollisionObject;
	
	UFUNCTION()
	void OnSnapperDeath(AActor* SnapperActor);
	
	TArray<FVector> GetSnapperSpawnPoints();

	UPROPERTY()
	TArray<TWeakObjectPtr<ASnapperEnemy>> SnappersBeingSpawned;

	// UFUNCTION()
	// void StartAttackAppearance();
	// UFUNCTION()
	// void PerformAttackAppearance();

	// Timeline functions for burrower appearing

	void StartHiding();
	void StartHidingHelper(float StartTime);
 
	void StartSurfacing();
	void StartSurfacingHelper();

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
	
	// void SetSurfacingHiding();
	// void SetDisappeared();
	// void SetAppeared();
	//void SetFollowing();

	FVector GetHidePosition();
	 
	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SurfacingStarted_CLI();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SurfacingEnded_CLI();
	UFUNCTION(BlueprintImplementableEvent) 
	void BP_HidingStarted_CLI();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_HidingEnded_CLI();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_HidingCancelled_CLI(); 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_WarningStarted_CLI(); 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_WarningEnded_CLI();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_FollowingStart_CLI();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_FollowingEnd_CLI();

	UFUNCTION(BlueprintCallable)
	FVector GetWorldWarningParticleSpawnLocation();

	UFUNCTION(BlueprintCallable) 
	FVector GetRelativeFollowParticleSpawnLocation();

	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	 
};
