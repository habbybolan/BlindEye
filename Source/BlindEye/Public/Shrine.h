// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Shrine.generated.h"

class UHealthComponent;

UCLASS()
class BLINDEYE_API AShrine : public AActor, public IHealthInterface, public IIndicatorInterface
{
	GENERATED_BODY()
	
public:	
	AShrine();

	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly)
	float MaxShrineHealth = 100.f;

	virtual float GetMass() override;

	DECLARE_MULTICAST_DELEGATE(FShrineHealthChangeSignature)
	FShrineHealthChangeSignature ShrineHealthChange;
	
	virtual float GetHealth() override;
	virtual void SetHealth(float NewHealth) override;
	virtual TEAMS GetTeam() override;
	virtual float GetHealthPercent() override;

	virtual void OnDeath(AActor* ActorThatKilled) override;
	virtual bool GetIsDead() override;
	virtual UHealthComponent* GetHealthComponent() override;

	UFUNCTION()
	void ChannelingStarted(ABlindEyeEnemyBase* EnemyChannelling);
	UFUNCTION()
	void ChannellingEnded(AActor* EnemyChannelling);

	virtual FVector GetIndicatorPosition() override;
 
protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnGameStarted();

	UPROPERTY()
	TSet<ABlindEyeEnemyBase*> EnemiesCurrentlyChanneling;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_HealthUpdated")
	float CurrShrineHealth;

	FTimerHandle ChargeUpdatingTimerHandle;
	float ChargeUpdatingDelay = 0.1f;
	UFUNCTION()
	void UpdateChargeUI();

	UFUNCTION()
	void OnRep_HealthUpdated();

	UFUNCTION()
	void OnChannellingEnemyDied(AActor* DeadChannellingEnemy);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateShrineCharge(float ChargePercent);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	
	virtual float GetMaxHealth() override;
	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType,
		AActor* DamageCauser) override;
	virtual UMarkerComponent* GetMarkerComponent() override;
};
