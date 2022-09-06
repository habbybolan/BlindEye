// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTypes/MarkData.h"
#include "Interfaces/DamageInterface.h"
#include "HealthComponent.generated.h"

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

protected:

	// cached owners health interface
	IHealthInterface* OwnerHealth;

	virtual void BeginPlay() override;
	
	FMarkData* CurrMark;

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
		
	FTimerHandle MarkerDecayTimerHandle;
	void RemoveMark();
		
};
