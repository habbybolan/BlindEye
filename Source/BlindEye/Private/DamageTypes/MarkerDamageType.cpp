// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkerDamageType.h"

float UMarkerDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation,
	UHealthComponent* HealthComponent) const
{
	return 1;
}
