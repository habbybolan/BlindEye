// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BurnStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"


void UBurnStatusEffect::ProcessEffect(AActor* Owner, APawn* HitCharacter, FVector HitLocation,
                                     UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Execute_Burn(HealthComponent, DamagePerSec, Duration);
	}
}