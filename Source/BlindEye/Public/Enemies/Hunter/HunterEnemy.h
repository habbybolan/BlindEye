// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Shrine.h"
#include "Components/TimelineComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "HunterEnemy.generated.h"

class UTimelineComponent;
UENUM(BlueprintType)
enum class EHunterAttacks : uint8
{
	None,
	BasicAttack,
	ChargedJump
};

class UBaseDamageType;

/**
 * 
 */
UCLASS()
class BLINDEYE_API AHunterEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()
	
public:

	AHunterEnemy(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	void Despawn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float AttackMaxWalkSpeed = 450;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float RunningMaxWalkSpeed = 600;

	UPROPERTY(EditDefaultsOnly, Category=Movement)  
	float JumpingBetweenIslandsRInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Channelling)
	float ChannellingDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Channelling)
	UAnimMontage* ChannelingAnim;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack, meta=(ClampMin=0))
	float BasicAttackDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=BasicAttack)
	float MaxDistanceForBasicAttack = 150.f;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack)
	TSubclassOf<UBaseDamageType> BasicAttackDamageTypeNoMark;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack)
	TSubclassOf<UBaseDamageType> BasicAttackDamageTypeWithMark;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack) 
	UAnimMontage* BasicAttackRightAnimation;
 
	UPROPERTY(EditDefaultsOnly, Category=BasicAttack) 
	UAnimMontage* BasicAttackLeftAnimation;

	UPROPERTY(EditDefaultsOnly, Category=BasicAttack, meta=(ClampMin=0, ClampMax=1))
	float MovementSlowOnBasicAttack = 0.5;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=1))
	float ChargedCooldown = 15.f;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=0, ToolTip="0 for infinite duration"))
	float ChargedDuration = 0;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=0, ClampMax=1, ToolTip="Percent of Damage to apply while Hunter not charged"))
	float UnchargedDamagePercent = 0.1;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=0, ClampMax=1, ToolTip="Percent of Damage to apply while hunter is charged"))
	float ChargedDamagePercent = 1;
 
	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=0, ClampMax=1, ToolTip="Percent of Damage to apply while Hunter is stunned"))
	float StunnedDamagePercent = 3;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ClampMin=0, ClampMax=1))
	float MovementSpeedAlteredDuringNotCharged = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category=Charged)
	UAnimMontage* RoarAnimation;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ToolTip="Applied when right on player, scaled with DistToMarkedPlayerToSlowDown", ClampMin=0, ClampMax=1))
	float MaxSlowAlterWhenCloseToPlayer = 0.5;

	UPROPERTY(EditDefaultsOnly, Category=Charged)
	float DistToMarkedPlayerToSlowDown = 300.f;

	UPROPERTY(EditDefaultsOnly, Category=Charged)
	float StunDurationOnMarkDetonated = 0.5f;
 
	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)
	float ChargedJumpCooldown = 10.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)
	float ChargedJumpDuration = 1.0f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ChargedJump)
	float MaxDistanceToChargeJump = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ChargedJump)
	float MinDistanceToChargeJump = 200.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)
	float ChargedAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)
	TSubclassOf<UBaseDamageType> ChargedJumpDamageTypeWithMark;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)  
	TSubclassOf<UBaseDamageType> ChargedJumpDamageTypeNoMark;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump)  
	UAnimMontage* ChargedJumpAnim; 

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	float ChargedJumpLandingDistanceBeforeTarget = 70.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	float ChargedJumpMaxPeakHeight = 500.f;
 
	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	float ChargedJumpMinPeakHeight = 50.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	float ChargedJumpRInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump, meta=(ClampMin=0.0001)) 
	float ChargedJumpCalcDelay = 0.02f;
 
	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	TArray<TEnumAsByte<EObjectTypeQuery>> ChargedJumpLOSBlockers;
	
	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	TEnumAsByte<EEasingFunc::Type> EasingForwardMovementChargedJump;

	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	TEnumAsByte<EEasingFunc::Type> EasingUpMovementChargedJump;
	
	UPROPERTY(EditDefaultsOnly, Category=ChargedJump) 
	TEnumAsByte<EEasingFunc::Type> EasingDownMovementChargedJump;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_IsVisible", BlueprintReadOnly)
	bool IsVisible = false;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* DissolveMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTimelineComponent* InvisTimelineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* InvisCurve;

	UFUNCTION()
	void OnRep_IsVisible();
	
	void PerformChargedJump();
	 
	void PerformBasicAttack();

	void TrySetVisibility(bool visibility);
	
	virtual void OnDeath(AActor* ActorThatKilled) override;

	bool GetIsChargedJumpOnCooldown();
	
	bool GetIsAttacking();
	EHunterAttacks GetCurrAttack();

	bool GetIsCharged(); 
	bool GetIsChannelling();
	
	void ApplyBasicAttackDamage(FHitResult Hit, bool IfShouldApplyHunterMark);
	void ApplyChargedJumpDamage(FHitResult Hit, bool IfShouldApplyHunterMark);

	void StartChanneling();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartChannelingHelper();
	void StopChanneling();

	void OnStunStart(float StunDuration);

	bool GetIsFleeing();

	void ChannelingAnimFinished();

	void SetFleeing();

	bool IsTargetMarked();

	bool IsTargetOnNavigableGround();
 
protected:

	bool bPlayerMarked = false;
 
	bool bFleeing = false;

	UPROPERTY(EditDefaultsOnly, Category=Charged)
	bool bCharged = false;

	UPROPERTY(EditDefaultsOnly, Category=Charged, meta=(ToolTip="Used for debugging to more easily stun the hunter"))
	bool bDebugStunOnMark = false; 
	
	bool bChannelling = false;

	FTimerHandle ChargedCooldownTimerHandle;
	FTimerHandle ChargedDurationTimerHandle;

	FTimerHandle ChannellingTimerHandle;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* Material;

	UPROPERTY()
	AShrine* Shrine;

	UPROPERTY()
	EHunterAttacks CurrAttack = EHunterAttacks::None;

	FOnTimelineFloat InvisUpdateEvent; 
	
	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformBasicAttackHelper(UAnimMontage* AnimToPlay);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChargedStarted();
  
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChargedEnded(); 
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChannelingStarted_CLI();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChannelingEnded_CLI();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_HunterDespawned_SER();

	void SetPlayerMarked(AActor* NewTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformChargedJumpHelper(FVector StartLoc, FVector EndLoc);

	UFUNCTION()
	void OnHunterMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType);

	UFUNCTION() 
	void OnHunterMarkRemoved(AActor* UnmarkedActor, EMarkerType MarkerType);

	UFUNCTION()
	void OnMarkedPlayerDied(AActor* PlayerDied);

	void UnsubscribeToTargetMarks();

	bool bChargeAttackCooldown = false;
	void SetChargedJumpOffCooldown();
	FTimerHandle ChargedJumpCooldownTimerHandle;

	float CurrTimeOfChargedJump = 0;	// Keeps track of how long of the jump the charged attack has been performed for
	FVector ChargedJumpTargetLocation;
	FVector ChargedJumpStartLocation;
	FTimerHandle PerformingChargedJumpTimerHandle;
	void PerformingJumpAttack();

	FTimerHandle ChargedJumpRotationTimerHandle; 
	UFUNCTION()
	void RotateDuringChargedAttack();

	float ChargedJumpPeakHeight; // Calculated peak height for charged jump based on distance to player
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetVisibility_CLI(bool visibility);

	void SetCharged();
	void SetNotCharged();

	void ApplyAttackDamageHelper(float Damage, bool IfShouldApplyHunterMark, TSubclassOf<UBaseDamageType> DamageType, FHitResult Hit);
	virtual void OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType) override;
	virtual void OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType) override;

	void RemoveHunterMarkOnPlayer();

	UFUNCTION()
	void AnimMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool bLeftBasicAttack = true;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	UFUNCTION()
	void TimelineInvisUpdate(float Value);

	void CalculateWalkSpeed();
	void RotateWhileJumping(float DeltaSeconds);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_HunterLeft();
};

