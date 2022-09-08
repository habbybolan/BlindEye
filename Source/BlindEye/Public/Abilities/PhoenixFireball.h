// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "PhoenixFireballCast.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "PhoenixFireball.generated.h"

// Jumping State
class BLINDEYE_API FPhoenixFireballCastState : public FAbilityState
{
public:
	FPhoenixFireballCastState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};

class UNiagaraComponent;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class BLINDEYE_API APhoenixFireball : public AAbilityBase
{
	GENERATED_BODY()

public:

	APhoenixFireball();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConeHalfAngleDeg = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConeWidth = 900;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConeLength = 750;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FireConeParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APhoenixFireballCast> FireballCastType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ConeTraceObjectTypes;

	// Deals with damage from the cone and the fireball. Fireball sends its damage event to this
	void DealWithDamage(AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit, float Damage);

	void CastFireCone();
	void CastFireball();
	
	
protected:

	UPROPERTY()
	UNiagaraComponent* SpawnedFireConeParticle;
	 
	TSet<uint32> IDsOfHitActors;

	virtual void EndAbilityLogic() override;
};
