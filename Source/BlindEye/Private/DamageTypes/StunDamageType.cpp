// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/StunDamageType.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

float UStunDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Execute_Stun(HealthComponent, StunDuration);
	}
	return DamageMultiplier;
}
