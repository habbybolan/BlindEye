// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "SnapperEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASnapperEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:
	ASnapperEnemy(const FObjectInitializer& ObjectInitializer);
	
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> JumpAttackDamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BasicAttackDamage = 5; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> BasicAttackDamageType;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll)
	UAnimMontage* GetUpFromBehindMontage;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll) 
	UAnimMontage* GetUpFromInFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll)
	float RagdollDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float DeathDelay = 1.0f;

	void PerformJumpAttack();
	void PerformBasicAttack(); 

	void TryRagdoll(bool SimulatePhysics);

	void TempLaunch();
	void LaunchSwing();

	UFUNCTION(BlueprintCallable)
	bool GetIsRagdolling();

	void ApplyKnockBack(FVector Force);

protected:

	UPROPERTY(Replicated)
	bool bRagdolling = false;
	FTimerHandle LaunchSwingTimerHandle;
	FTimerHandle ColliderOnMeshTimerHandle;
	FTimerHandle StopRagdollTimerHandle;
	FTimerHandle GetupAnimTimerHandle;
	FTimerHandle DeathTimerHandle;

	float AlphaBlendWeight = 1;

	void BeginStopRagdollTimer();

	void TeleportColliderToMesh();

	UPROPERTY(Replicated)
	FVector HipLocation;

	// Only called from client to replicate the hip location while ragdolling
	void UpdateHipLocation();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartRagdoll();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StopRagdoll();
	void FinishGettingUp();

	void SetPhysicsBlendWeight();
	FTimerHandle PhysicsBlendWeightTimerHandle;
	float BlendWeightDelay = 0.02f;

	bool IsLayingOnFront();

	virtual void OnDeath(AActor* ActorThatKilled) override;
	void DestroySnapper();

	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	
};
