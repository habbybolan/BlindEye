// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BurnStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"


void UBurnStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                     UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Burn(DamagePerSec, Duration, Owner);
	}
}
