// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/StunDamageType.h"

float UStunDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation) const
{
	return Super::ProcessDamage(Owner, HitCharacter, HitLocation);
	// TODO:
}
