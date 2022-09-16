// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/KnockBackStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UKnockBackStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		FVector direction = HitCharacter->GetActorLocation() - HitLocation;
		direction.Normalize();
		direction *= KnockBackForce;
		direction.Z = KnockUpForce;
		DamageInterface->Execute_KnockBack(HealthComponent, direction, Owner);
	}
}
