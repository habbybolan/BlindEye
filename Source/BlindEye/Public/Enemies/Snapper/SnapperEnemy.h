// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "SnapperEnemy.generated.h"

UENUM(BlueprintType) 
enum class ESnapperAttacks : uint8
{
	None,
	BasicAttack, 
	JumpAttack
};

class UShrineAttackPoint;
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

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float JumpingBetweenIslandsRInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float BaseMoveSpeed = 450.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	float AttackingShrineMoveSpeed = 200.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump Attack")
	float JumpAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump Attack")
	UAnimMontage* JumpAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category="Jump Attack")
	float DistanceToJumpAttack = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float DistanceToBasicAttack = 200.f;
    	  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	float BasicAttackDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Basic Attack")
	UAnimMontage* BasicAttackAnim;
	
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* RagdollCapsule;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll)
	UAnimMontage* GetUpFromBehindMontage;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll) 
	UAnimMontage* GetUpFromInFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll)
	float RagdollDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category=Ragdoll)
	UAnimMontage* GetupRoarAnim;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.1, ClampMax=1), Category=Spawning)
	float GravityScaleAlteredOnSpawn = 0.25f;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.1, ClampMax=1), Category=Spawning)
	float ColliderHeightAlteredOnSpawn = 0.5f;

	bool IsBasicAttackOnDelay = false;
	bool IsJumpAttackOnDelay = false;

	ESnapperAttacks CurrAttack = ESnapperAttacks::None;

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

	void StopJumpAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StopJumpAttackHelper();
	void StopBasicAttack();

	bool IsAttackingShrine = true; 

	UPROPERTY(Replicated)
	FVector QueuedSpawnForce;

	void UnsubFromShrineAttackPoint();
	void SubToShrineAttackPoint(UShrineAttackPoint* ShrineAttackPoint);
	UShrineAttackPoint* GetShrineAttackPoint();
	
protected:

	UPROPERTY()
	UShrineAttackPoint* SubscribedShrineAttackPoint;

	UPROPERTY(Replicated)
	bool bRagdolling = false;
	UPROPERTY(Replicated)
	bool bGettingUp = false;

	UPROPERTY(Replicated)
	bool bIsSpawning = true;
	UFUNCTION()
	void CollisionWithGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	FTimerHandle LaunchSwingTimerHandle;
	FTimerHandle StopRagdollTimerHandle;
	FTimerHandle GetupAnimTimerHandle;
	
	FTimerHandle JumpAttackDelayTimerHandle;
	FTimerHandle BasicAttackDelayTimerHandle;

	float AlphaBlendWeight = 1;
	
	void BeginStopRagdollTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnSpawnCollisionHelper();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformBasicAttackHelper();
	
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

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PerformJumpAttackHelper();

	UFUNCTION()
	void OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual void OnDeath(AActor* ActorThatKilled) override;
	
	void SetCanJumpAttack();
	void SetCanBasicAttack();

	void RotateWhileJumping(float DeltaSeconds);

	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnPlayerAttackedSnapper();
	
};
