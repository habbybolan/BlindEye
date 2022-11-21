// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DamageTypes/BaseDamageType.h"
#include "GameFramework/Actor.h"
#include "BurrowerProjectile.generated.h"

class UProjectileMovementComponent;
UCLASS()
class BLINDEYE_API ABurrowerProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABurrowerProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialAngle = 45.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	float DestroyDelay = 2.f;

	UFUNCTION(NetMulticast, Reliable)
	void LaunchProjectile(FVector TargetPosition);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnContact();

private: 

	FTimerHandle DelayedDeathTimerHandle;
	UFUNCTION()
	void DelayedDestruction();

	UFUNCTION()
	void OnContact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
