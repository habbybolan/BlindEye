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

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* RagdollCapsule;
	
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

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.1, ClampMax=1), Category=Spawning)
	float GravityScaleAlteredOnSpawn = 0.25f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.1, ClampMax=1), Category=Spawning)
	float ColliderHeightAlteredOnSpawn = 0.5f; 

	void PerformJumpAttack();
	void PerformBasicAttack(); 

	UFUNCTION(BlueprintCallable) 
	void TryRagdoll(bool SimulatePhysics, bool IsIndefiniteRagdoll = false);

	void TempLaunch();
	void LaunchSwing();

	UFUNCTION(BlueprintCallable)
	bool GetIsRagdolling();

	UFUNCTION(BlueprintCallable)
	bool GetIsSpawning(); 

	void ApplyKnockBack(FVector Force);

	void ManualStopRagdollTimer(float Duration); 

protected:

	UPROPERTY(Replicated)
	bool bRagdolling = false;
	UPROPERTY(Replicated)
	bool bGettingUp = false;

	UPROPERTY(Replicated)
	bool bIsSpawning = true;
	UFUNCTION()
	void SpawnCollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	FTimerHandle LaunchSwingTimerHandle;
	FTimerHandle StopRagdollTimerHandle;
	FTimerHandle GetupAnimTimerHandle;

	float AlphaBlendWeight = 1;

	void BeginStopRagdollTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnSpawnCollisionHelper();
	
	ECollisionChannel CachedCollisionObject;

	void TeleportColliderToMesh(float DeltaSeconds);

	UPROPERTY(Replicated)
	FVector HipLocation;

	// Only called from client to replicate the hip location while ragdolling
	void UpdateHipLocation(float DeltaSeconds); 

	UFUNCTION(NetMulticast, Reliable) 
	void MULT_StartRagdoll(bool IsIndefiniteRagdoll);
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StopRagdoll();
	void FinishGettingUp();

	void SetPhysicsBlendWeight();
	FTimerHandle PhysicsBlendWeightTimerHandle;
	float BlendWeightDelay = 0.02f;

	bool IsLayingOnFront();

	virtual void OnDeath(AActor* ActorThatKilled) override;

	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	
};
