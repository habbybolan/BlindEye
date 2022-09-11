// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/StaggerStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UStaggerStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Execute_Stagger(HealthComponent);
	}
}
