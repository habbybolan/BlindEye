// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/KnockBackStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UKnockBackStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		FVector direction = HitCharacter->GetActorLocation() - HitLocation;
		direction.Z = 1;
		direction.Normalize();
		direction *= KnockBackForce;
		direction.Z = KnockUpForce;
		DamageInterface->KnockBack(direction, Owner);
	}
}
