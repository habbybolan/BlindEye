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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireballSpeed = 100;

	UPROPERTY(EditDefaultsOnly)
	float BurningBaseDamagePerSec = 1.f;

	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Delay between damage ticks for enemy in burning pool"))
	float BurnDamageDelay = 0.5; 

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

	DECLARE_DYNAMIC_DELEGATE(FCustomCollisionSignaure);
	FCustomCollisionSignaure CustomCollisionDelegate;

	USphereComponent* GetSphereComponent();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Explosion_CLI();
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_GroundBurning_CLI(FVector LocationForGroundBurn, float Duration);

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	UFUNCTION()
	void DelayedDestruction();
	
	void HideFireball(); 

	// called continually until actor deleted for applying burn at location
	UFUNCTION()
	void BurnLogic();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCollision_CLI();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void CollisionLogic();

	FTimerHandle BurnTimerHandle;
	FTimerHandle DelayedDestroyTimerHandle;
	FTimerHandle LifespanTimerHandle;

	FVector BurnLocation;
};
