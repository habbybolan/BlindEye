// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BaseDamageType.h"

float UBaseDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation) const
{
	return DamageMultiplier;
	
	// TODO: Check for mark-able interface and add extra mark damage?
}
