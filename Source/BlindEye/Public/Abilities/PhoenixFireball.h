// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "PhoenixFireballCast.h"
#include "Abilities/AbilityBase.h"
#include "DamageTypes/BaseDamageType.h"
#include "PhoenixFireball.generated.h"

// Use Ability State
class BLINDEYE_API FStartCastingAbilityState : public FAbilityState
{
public:
	FStartCastingAbilityState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};
 
// End casting ability
class BLINDEYE_API FCastFireballState : public FAbilityState
{
public:
	FCastFireballState(AAbilityBase* ability);
	virtual void TryEnterState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void RunState(EAbilityInputTypes abilityUsageType = EAbilityInputTypes::None) override;
	virtual void ExitState() override;
};
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float CostPercent = 30;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=1))
	float SlowAmount = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APhoenixFireballCast> FireballCastType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ConeTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireballCastAnimation;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> FireballCastObjectTypes;
	
	// Deals with damage from the cone and the fireball. Fireball sends its damage event to this
	void DealWithDamage(AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit, float Damage);

	void CastFireCone();
	void CastFireball();

	UFUNCTION()
	void OnFireballCastHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Wait for ability use animation notify to cast fireball
	void PlayAbilityAnimation();
	
	
protected:
	 
	TSet<uint32> IDsOfHitActors;
	UPROPERTY()
	APhoenixFireballCast* FireballCast;

	virtual void EndAbilityLogic() override;

	UFUNCTION()
	void UseAnimNotifyExecuted();
 
	UFUNCTION()
	void EndAnimationNotifyExecuted();
};
