// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BurnDamageType.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"


float UBurnDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation,
                                     UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Execute_Burn(HealthComponent, DamagePerSec, Duration);
	}
	return DamageMultiplier;
}
