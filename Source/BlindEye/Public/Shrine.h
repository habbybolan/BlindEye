// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Shrine.generated.h"

UCLASS()
class BLINDEYE_API UShrineAttackPoint : public UObject
{
	GENERATED_BODY()

public:
	
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
		if (SubscribedSnapper.IsValid())
		{
			UnsubscribeSnapper();
		}
		SubscribedSnapper = MakeWeakObjectPtr(Snapper);
		Snapper->SubToShrineAttackPoint(this);
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
		if (SubscribedSnapper.IsValid())
		{
			ASnapperEnemy* PrevSnapper = SubscribedSnapper.Get();
			PrevSnapper->UnsubFromShrineAttackPoint();
			SubscribedSnapper.Get()->GetHealthComponent()->OnDeathDelegate.RemoveDynamic(this, &UShrineAttackPoint::OnSnapperDeath);
		}
		SubscribedSnapper = nullptr;
	}

	void PerformShift(ASnapperEnemy* NewSnapper)
	{
		if (SubscribedSnapper.IsValid())
		{
			UnsubscribeSnapper();
		}
		check(NewSnapper);
		SubscribeSnapper(NewSnapper);
	}

	bool GetIsFree()
	{
		return !SubscribedSnapper.IsValid();
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
	float AttackPointDistOffset = 15;

	UPROPERTY(EditDefaultsOnly, Category=Ticketing)
	float AttackPointCenterOffsetRight = -55;
	
	UPROPERTY(EditDefaultsOnly, Category=Ticketing)
	float AttackPointCenterOffsetForward = 0;

	UPROPERTY(EditDefaultsOnly, Category=Ticketing)
	bool bShowAttackingPoints = false;

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

	/**
	 * Checks for the closest shrine attack point and checks if a valid spot exists
	 * @param Snapper	Snapper asking for point to subscribe to to attack shrine
	 * returns			nullptr if no points available, otherwise returns the closest point to subscribe to
	 */
	UShrineAttackPoint* AskForClosestPoint(ASnapperEnemy* Snapper);

	uint8 FindClosestAttackPointIndex(const FVector& AskerLocation);
	bool IsOpenAttackPoint();
	void PerformShift(int8 AttackPointIndex, ASnapperEnemy* AskingSnapper);
	uint8 GetClosestOpenPointLeft(uint8 AttackPointIndex, bool& IsRightClosest);
 
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
