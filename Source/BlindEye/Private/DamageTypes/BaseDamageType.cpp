// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BaseDamageType.h"
#include "Components/HealthComponent.h"
#include "DamageTypes/BaseStatusEffect.h"

UBaseDamageType::UBaseDamageType()
{
}

UBaseDamageType::~UBaseDamageType()
{
}

float UBaseDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	for (TSubclassOf<UBaseStatusEffect> statusEffectType : StatusEffects)
	{
		UBaseStatusEffect* statusEffect = NewObject<UBaseStatusEffect>(GetTransientPackage(), statusEffectType);
		//TUniquePtr<UBaseStatusEffect> statusEffect = TUniquePtr<UBaseStatusEffect>(NewObject<UBaseStatusEffect>(GetTransientPackage(), statusEffectType));
		statusEffect->ProcessEffect(Owner, HitCharacter, HitLocation, HealthComponent);
		//delete statusEffect;
	}
	return DamageMultiplier;
}
