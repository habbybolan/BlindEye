// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
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
	float Damage = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurnDamagePerSec = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningDuration = 4.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightToApplyFire = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FireTrailParticle;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* GroundBurnParticle;

	USphereComponent* GetSphereComponent();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnFireballTrail(); 
	
	UFUNCTION()
	void DelayedDestruction();

	// called continually until actor deleted for applying burn at location
	UFUNCTION()
	void BurnLogic();

	UPROPERTY()
	UNiagaraComponent* SpawnedFireTrailParticle;
 
	UPROPERTY()
	UNiagaraComponent* SpawnedGroundBurnParticle;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FTimerHandle BurnTimerHandle;
	FTimerHandle DelayedDestroyTimerHandle;
};
