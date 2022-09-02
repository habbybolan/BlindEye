// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/DamageInterface.h"
#include "Interfaces/HealthInterface.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UHealthComponent : public UActorComponent, public IDamageInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:

	// cache owners health interface
	IHealthInterface* OwnerHealth;

	virtual void BeginPlay() override;

	void SetPointDamage(AActor* DamagedActor, float Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);
	
	void SetRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser);

	void SetDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	virtual void Stun_Implementation() override;

	virtual void KnockBack_Implementation() override;

	virtual void Burn_Implementation() override;

	virtual void Stagger_Implementation() override;
		
};
