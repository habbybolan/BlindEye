// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/TauntStatusEffect.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UTauntStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                       UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Owner->GetInstigator()))
		{
			// TODO: Pass player into taunt
			DamageInterface->Execute_TryTaunt(HealthComponent, Duration, Owner);
		}
	}
}
