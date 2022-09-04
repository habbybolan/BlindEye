// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/DetonatorDamageType.h"

#include "Characters/BlindEyeCharacter.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

float UDetonatorDamageType::ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation,
                                          UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyeCharacter* Player = Cast<ABlindEyeCharacter>(Owner))
		{
			DamageInterface->Execute_TryDetonation(HealthComponent, Player->PlayerType);
		}
	}
	return 0;
}
