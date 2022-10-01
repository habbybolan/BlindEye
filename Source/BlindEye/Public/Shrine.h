// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Shrine.generated.h"

class UHealthComponent;

UCLASS()
class BLINDEYE_API AShrine : public AActor, public IHealthInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShrine();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly)
	float MaxShrineHealth = 100.f;

	DECLARE_MULTICAST_DELEGATE(FShrineHealthChangeSignature)
	FShrineHealthChangeSignature ShrineHealthChange;
	
	virtual float GetHealth() override;
	virtual void SetHealth(float NewHealth) override;
	virtual TEAMS GetTeam() override;
	virtual float GetHealthPercent() override;

	virtual void OnDeath(AActor* ActorThatKilled) override;
	virtual bool GetIsDead() override;
	virtual UHealthComponent* GetHealthComponent() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_HealthUpdated")
	float CurrShrineHealth;

	UFUNCTION()
	void OnRep_HealthUpdated();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	
	virtual float GetMaxHealth() override;
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType,
		AActor* DamageCauser) override;
	virtual UMarkerComponent* GetMarkerComponent() override;
};
