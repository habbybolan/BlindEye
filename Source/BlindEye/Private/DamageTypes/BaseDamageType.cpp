// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BaseDamageType.h"
#include "Components/HealthComponent.h"

float UBaseDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	// TODO: Apply effects
	return DamageMultiplier;
}
