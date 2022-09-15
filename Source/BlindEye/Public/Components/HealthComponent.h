// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTypes/BaseDamageType.h"
#include "DamageTypes/MarkData.h"
#include "Interfaces/DamageInterface.h"
#include "HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FAppliedStatusEffects 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStun = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsBurn = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsMarked = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStaggered = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsTaunt = false;

	// keep track of burn damage
	float BurnDPS = 0;
};

enum class PlayerType : uint8;
class IHealthInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UHealthComponent : public UActorComponent, public IDamageInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MarkerDecay = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RefreshMarkerAmount = 2.f;

	UFUNCTION(BlueprintCallable)
	const FAppliedStatusEffects& GetAppliedStatusEffect();

	DECLARE_MULTICAST_DELEGATE_OneParam(FDeathSignature, AActor*)
	FDeathSignature OnDeathDelegate;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, ReplicatedUsing="OnRep_IsInvincibility")
	bool IsInvincible = false;

	UFUNCTION()
	void OnRep_IsInvincibility();

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

	DECLARE_MULTICAST_DELEGATE_OneParam(FMarkedSignature, PlayerType) 
	FMarkedSignature MarkedAddedDelegate;
	DECLARE_MULTICAST_DELEGATE(FUnMarkedSignature) 
	FUnMarkedSignature MarkedRemovedDelegate; 

	DECLARE_MULTICAST_DELEGATE(FDetonateSignature) 
	FDetonateSignature DetonateDelegate;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FTauntStartSignature, float, AActor*) 
	FTauntStartSignature TauntStartDelegate;
	DECLARE_MULTICAST_DELEGATE(FTauntEndSignature) 
	FTauntEndSignature TauntEndDelegate; 

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnDeath();

	// cached owners health interface
	IHealthInterface* OwnerHealth;

	virtual void BeginPlay() override;

	// Status effect properties ***
	
	FMarkData* CurrMark;
	FAppliedStatusEffects AppliedStatusEffects;

	FTimerHandle StunTimerHandle; 
	FTimerHandle BurnTimerHandle; 
	FTimerHandle BurnAppliedTimerHandle;
	FTimerHandle TauntTimerHandle;

	float DelayBetweenBurnTicks = 0.2f;

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
	void SetPointDamage(AActor* DamagedActor, float Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void SetRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser);

	void SetDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	virtual void Stun_Implementation(float StunDuration, AActor* DamageCause) override;

	virtual void KnockBack_Implementation(FVector KnockBackForce, AActor* DamageCause) override;

	virtual void Burn_Implementation(float DamagePerSec, float Duration, AActor* DamageCause) override;

	virtual void Stagger_Implementation(AActor* DamageCause) override;

	virtual void TryApplyMarker_Implementation(PlayerType Player, AActor* DamageCause) override;

	virtual void TryDetonation_Implementation(PlayerType Player, AActor* DamageCause) override;
 
	virtual void TryTaunt_Implementation(float Duration, AActor* Taunter) override;

	// Detonation Effect properties *********
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	TSubclassOf<UBaseDamageType> DarkDetonationOnEnemyDamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	float DarkDetonationOnEnemyDamage = 15;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	TSubclassOf<UBaseDamageType> FireDetonationOnEnemyDamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detonation")
	TSubclassOf<UBaseDamageType> DarkDetonationOnPlayerDamageType;  

	// End Detonation Effect properties *********
	
	FTimerHandle MarkerDecayTimerHandle;
	void RemoveMark();
	void DetonateMark();
	void PerformDetonationEffect(AActor* DamageCause); 
};
