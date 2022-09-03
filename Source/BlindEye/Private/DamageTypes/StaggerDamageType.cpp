// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/StaggerDamageType.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

float UStaggerDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Execute_Stagger(HealthComponent);
	}
	return DamageMultiplier;
}
