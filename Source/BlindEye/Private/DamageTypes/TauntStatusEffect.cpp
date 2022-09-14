// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/TauntStatusEffect.h"

#include "Characters/BlindEyeCharacter.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UTauntStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                       UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyeCharacter* Player = Cast<ABlindEyeCharacter>(Owner->GetInstigator()))
		{
			// TODO: Pass player into taunt
			DamageInterface->Execute_TryTaunt(HealthComponent, Duration, Owner);
		}
	}
}
