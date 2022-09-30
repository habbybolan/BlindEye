// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DamageTypes/BaseDamageType.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhoenixFireballCast.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class BLINDEYE_API APhoenixFireballCast : public AActor
{
	GENERATED_BODY()
	
public:
	
	APhoenixFireballCast();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireballSpeed = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurnDamagePerSec = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningDuration = 4.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightToApplyFire = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> BurnDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TArray<TEnumAsByte<EObjectTypeQuery>> BurnObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FireTrailParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CustomLifespan = 4.0f;

	USphereComponent* GetSphereComponent();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Explosion();
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_GroundBurning(FVector LocationForGroundBurn, float Duration);

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnFireballTrail(); 
	
	UFUNCTION()
	void DelayedDestruction();

	// called continually until actor deleted for applying burn at location
	UFUNCTION()
	void BurnLogic();

	UPROPERTY()
	UNiagaraComponent* SpawnedFireTrailParticle;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void CollisionLogic();

	FTimerHandle BurnTimerHandle;
	FTimerHandle DelayedDestroyTimerHandle;
	FTimerHandle LifespanTimerHandle;

	FVector BurnLocation;
};
