// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/DetonatorStatusEffect.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UDetonatorStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                          UHealthComponent* HealthComponent) const
{
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Owner->GetInstigator()))
		{
			DamageInterface->TryDetonation(Player->PlayerType, Owner);
		}
	}
}
