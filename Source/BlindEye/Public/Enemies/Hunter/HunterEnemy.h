// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Shrine.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "HunterEnemy.generated.h"

UENUM(BlueprintType)
enum class EHunterAttacks : uint8
{
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

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float AttackMaxWalkSpeed = 450;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float StalkingMaxWalkSpeed = 200;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float RunningMaxWalkSpeed = 600;

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
	UAnimMontage* BasicAttackAnimation;

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

	UPROPERTY(BlueprintReadWrite)
	bool IsVisible = false;
	
	void PerformChargedJump();
	 
	void PerformBasicAttack();

	void TrySetVisibility(bool visibility);
	
	virtual void OnDeath(AActor* ActorThatKilled) override;

	bool GetIsChargedJumpOnCooldown();
	
	bool GetIsAttacking();

	bool GetIsCharged(); 
	bool GetIsChannelling();
	
	void ApplyBasicAttackDamage(FHitResult Hit, bool IfShouldApplyHunterMark);
	void ApplyChargedJumpDamage(FHitResult Hit, bool IfShouldApplyHunterMark);

	void StartChanneling();
	void StopChanneling();

	void OnStunStart(float StunDuration);
	void OnStunEnd();

	bool GetIsFleeing();

	void ChannelingAnimFinished();

	void SetAttackFinished();
 
protected:

	bool bAttacking = false;
	bool bPlayerMarked = false;
 
	bool bFleeing = false;

	UPROPERTY(EditDefaultsOnly, Category=Charged)
	bool bCharged = false;
	
	bool bChannelling = false;
	float CachedRunningSpeed;

	FTimerHandle ChargedCooldownTimerHandle;
	FTimerHandle ChargedDurationTimerHandle;

	FTimerHandle ChannellingTimerHandle;

	UPROPERTY()
	AShrine* Shrine;
	
	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformBasicAttackHelper();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChargedStarted();
  
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ChargedEnded();

	void SetPlayerMarked(AActor* NewTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformChargedJumpHelper(FVector StartLoc, FVector EndLoc);

	UFUNCTION()
	void OnHunterMarkDetonated();

	UFUNCTION() 
	void OnHunterMarkRemoved();

	void UnsubscribeToTargetMarks();

	bool bChargeAttackCooldown = false;
	void SetChargedJumpOffCooldown();
	FTimerHandle ChargedJumpCooldownTimerHandle;

	float CurrTimeOfChargedJump = 0;	// Keeps track of how long of the jump the charged attack has been performed for
	FVector ChargedJumpTargetLocation;
	FVector ChargedJumpStartLocation; 
	FTimerHandle PerformingChargedJumpTimerHandle;
	void PerformingJumpAttack();

	// Intermediary method to make RPC call to blueprint implementable method
	UFUNCTION(NetMulticast, Reliable)
	void MULT_TurnVisible(bool visibility);
 
	UFUNCTION(BlueprintImplementableEvent)
	void TrySetVisibiltiyHelper(bool visibility);

	void SetCharged();
	void SetNotCharged();

	void ApplyAttackDamageHelper(float Damage, bool IfShouldApplyHunterMark, TSubclassOf<UBaseDamageType> DamageType, FHitResult Hit);
	virtual void OnMarkDetonated() override;
	virtual void OnMarkAdded(EMarkerType MarkerType) override;

	void RemoveHunterMarkOnPlayer();
};

