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
	float BurningDuration = 4.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightToApplyFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FireTrailParticle;

	USphereComponent* GetSphereComponent();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_SpawnFireballTrail(); 
	
	UFUNCTION()
	void DelayedDestruction();

	UPROPERTY()
	UNiagaraComponent* SpawnedFireTrailParticle;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FTimerHandle DelayedDestroyTimerHandle;
};
