// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/HealingWell.h"
#include "Components/ActorComponent.h"
#include "DamageTypes/BaseDamageType.h"
#include "DamageTypes/MarkData.h"
#include "Interfaces/DamageInterface.h"
#include "HealthComponent.generated.h"

enum class EMarkerType : uint8;

USTRUCT(BlueprintType)
struct FAppliedStatusEffects 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStun = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsBurn = false;
	// keep track of burn damage
	float BurnDPS = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsMarked = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStaggered = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsTaunt = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsImprovedHealing = false;
	float HealPercentIncrease = 0;

	
};

enum class EPlayerType : uint8;
class IHealthInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UHealthComponent : public UActorComponent, public IDamageInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent(const FObjectInitializer& ObjectInitializer);
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MarkerDecay = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RefreshMarkerAmount = 2.f;

	virtual void AddMarkerHelper(EMarkerType MarkerType);

	UFUNCTION(BlueprintCallable)
	const FAppliedStatusEffects& GetAppliedStatusEffect();
 
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathSignature, AActor*, EnemyKilled);
	UPROPERTY()
	FDeathSignature OnDeathDelegate;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool IsInvincible = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealPerSec = 0;

	void Kill();

	// Delegates for status effects*********
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FStunStartSignature, float)
	FStunStartSignature StunStartDelegate;
	DECLARE_MULTICAST_DELEGATE(FStunEndSignature)
	FStunEndSignature StunEndDelegate; 

	DECLARE_MULTICAST_DELEGATE_OneParam(FKnockBackSignature, FVector)
	FKnockBackSignature KnockBackDelegate;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FBurnStartSignature, float, float) 
	FBurnStartSignature BurnDelegateStart;
	DECLARE_MULTICAST_DELEGATE(FBurnEndSignature) 
	FBurnEndSignature BurnDelegateEnd; 

	DECLARE_MULTICAST_DELEGATE(FStaggerSignature) 
	FStaggerSignature StaggerDelegate;

	DECLARE_MULTICAST_DELEGATE_OneParam(FMarkedSignature, EMarkerType) 
	FMarkedSignature MarkedAddedDelegate;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnMarkedSignature);
	UPROPERTY()
	FUnMarkedSignature MarkedRemovedDelegate; 

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDetonateSignature);
	UPROPERTY()
	FDetonateSignature DetonateDelegate;
 
	DECLARE_MULTICAST_DELEGATE(FRefreshSignature) 
	FRefreshSignature RefreshMarkDelegate;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FTauntStartSignature, float, AActor*) 
	FTauntStartSignature TauntStartDelegate;
	DECLARE_MULTICAST_DELEGATE(FTauntEndSignature) 
	FTauntEndSignature TauntEndDelegate;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FImprovedHealingStartSignature, float, float)
	FImprovedHealingStartSignature ImprovedHealingStartDelegate;
	DECLARE_MULTICAST_DELEGATE(FImprovedHealingEndSignature)  
	FImprovedHealingEndSignature ImprovedHealingEndDelegate;

	virtual void ImprovedHealing(float HealPercentIncrease, float Duration) override;
	FTimerHandle ImprovedHealingTimerHandle;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// TSubclassOf<AHealingWell> HealingWellType;

	void RemoveMark();
	FMarkData& GetCurrMark();

	virtual void KnockBack(FVector KnockBackForce, AActor* DamageCause) override;


	// Check if enemy is marked by Hunter
	bool GetIsHunterDebuff();

	virtual void Stun(float StunDuration, AActor* DamageCause) override;

	virtual void Burn(float DamagePerSec, float Duration, AActor* DamageCause) override;

	virtual void Stagger(AActor* DamageCause) override;

	virtual void TryApplyMarker(EMarkerType Player, AActor* DamageCause) override;

	virtual void TryDetonation(EPlayerType Player, AActor* DamageCause) override;
 
	virtual void TryTaunt(float Duration, AActor* Taunter) override;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnDeath(AActor* ActorThatKilled);

	// cached owners health interface
	IHealthInterface* OwnerHealth;

	virtual void BeginPlay() override;

	// Status effect properties ***

	UPROPERTY(Replicated)
	FMarkData CurrMark;
	
	FAppliedStatusEffects AppliedStatusEffects;

	FTimerHandle StunTimerHandle; 
	FTimerHandle BurnTimerHandle; 
	FTimerHandle BurnAppliedTimerHandle;
	FTimerHandle TauntTimerHandle;

	float DelayBetweenBurnTicks = 0.2f;

	FTimerHandle HealTimerHandle;
	float PerformHealDelay = 0.2f;
	void PerformHeal();

	// End Status effect properties ***

	UFUNCTION()
	void RemoveStun();
	UFUNCTION()
	void RemoveBurn();
	UFUNCTION()
	void ApplyBurn(); 
	UFUNCTION()
	void RemoveTaunt();
	UFUNCTION()
	void RemoveImprovedHealing();

	UFUNCTION()
	void SetPointDamage(AActor* DamagedActor, float Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void SetRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser);

	void SetDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);
	virtual float ProcessDamage(float Damage); 

	// Detonation Effect properties *********
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// TSubclassOf<UBaseDamageType> DarkDetonationOnEnemyDamageType;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// float DarkDetonationOnEnemyDamage = 15;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// TSubclassOf<UBaseDamageType> FireDetonationOnEnemyDamageType;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// TSubclassOf<UBaseDamageType> DarkDetonationOnPlayerDamageType;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// float DarkDetonationOnPlayerDamage = 35;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	// float DarkDetonationOnPlayerRadius = 450;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Detonation)
	float ExtraDetonationDamage = 30.f;
 
	// End Detonation Effect properties *********
	
	FTimerHandle MarkerDecayTimerHandle;

	virtual void DetonateMark();
	void PerformDetonationEffect(AActor* DamageCause); 

	void NotifyPlayersOnDetonation();
};
