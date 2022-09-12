// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/DetonatorStatusEffect.h"

#include "Characters/BlindEyeCharacter.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UDetonatorStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                          UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyeCharacter* Player = Cast<ABlindEyeCharacter>(Owner->GetInstigator()))
		{
			DamageInterface->Execute_TryDetonation(HealthComponent, Player->PlayerType);
		}
	}
}
