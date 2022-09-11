// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// cached owners health interface
	IHealthInterface* OwnerHealth;

	virtual void BeginPlay() override;
	
	FMarkData* CurrMark;
	FAppliedStatusEffects AppliedStatusEffects;

	UFUNCTION()
	void SetPointDamage(AActor* DamagedActor, float Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void SetRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser);

	void SetDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	virtual void Stun_Implementation(float StunDuration) override;

	virtual void KnockBack_Implementation(FVector KnockBackForce) override;

	virtual void Burn_Implementation(float DamagePerSec, float Duration) override;

	virtual void Stagger_Implementation() override;

	virtual void TryApplyMarker_Implementation(PlayerType Player) override;

	virtual void TryDetonation_Implementation(PlayerType Player) override;

	virtual void TryTaunt_Implementation(float Duration) override;
		
	FTimerHandle MarkerDecayTimerHandle;
	void RemoveMark();
		
};
