// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Shrine.generated.h"

UENUM()
enum EShrineAttackPointState
{
	Empty,
	Queued,
	Taken
};

UCLASS()
class BLINDEYE_API UShrineAttackPoint : public UObject
{
	GENERATED_BODY()

public:

	EShrineAttackPointState State = EShrineAttackPointState::Empty;
	TWeakObjectPtr<ASnapperEnemy> SubscribedSnapper = nullptr;
	FVector Location;

	UShrineAttackPoint() {}

	// Initialize attack point
	void Initialize(FVector location)
	{
		Location = location;
	}
	
	void SubscribeSnapper(ASnapperEnemy* Snapper)
	{
		SubscribedSnapper = MakeWeakObjectPtr(Snapper);
		Snapper->GetHealthComponent()->OnDeathDelegate.AddDynamic(this, &UShrineAttackPoint::OnSnapperDeath);
	}

	UFUNCTION()
	void OnSnapperDeath(AActor* EnemyKilled)
	{
		UnsubscribeSnapper();
	}

	// Ubsubscribe a snapper from the attack point (Manually left or snapper died)
	void UnsubscribeSnapper()
	{
		State = EShrineAttackPointState::Empty;
		SubscribedSnapper = nullptr;
	}
};

class UHealthComponent;

UCLASS()
class BLINDEYE_API AShrine : public AActor, public IHealthInterface, public IIndicatorInterface
{
	GENERATED_BODY()
	
public:	
	AShrine();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* IndicatorPosition;

	UPROPERTY(EditDefaultsOnly)
	float MaxShrineHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, Category=Ticketing)
	uint8 NumSurroundingAttackPoints = 10;
	
	UPROPERTY(EditDefaultsOnly, Category=Ticketing)
	uint8 AttackPointDistOffset = 15;

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
 
	void StartWaitingForPlayersToInteract();
 
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

	UPROPERTY()
	TArray<UShrineAttackPoint*> AttackPoints;

	void InitializeAttackPoint();
	
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
