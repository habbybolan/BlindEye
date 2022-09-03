// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BaseDamageType.h"
#include "Components/HealthComponent.h"

float UBaseDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	return DamageMultiplier;
	
	// TODO: Check for mark-able interface and add extra mark damage?
}
