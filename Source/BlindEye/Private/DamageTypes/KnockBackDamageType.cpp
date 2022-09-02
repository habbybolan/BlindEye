// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/KnockBackDamageType.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

float UKnockBackDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		FVector direction = HitCharacter->GetActorLocation() - HitLocation;
		direction.Normalize();
		direction *= KnockBackForce;
		direction.Z = KnockUpForce;
		DamageInterface->Execute_KnockBack(HealthComponent, direction);
	}
	return DamageMultiplier;
}
