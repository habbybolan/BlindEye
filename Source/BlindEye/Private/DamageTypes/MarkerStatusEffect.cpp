// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkerStatusEffect.h"

#include "Components/HealthComponent.h"
#include "Characters/BlindEyeCharacter.h"
#include "Interfaces/DamageInterface.h"

void UMarkerStatusEffect::ProcessEffect(AActor* Owner, APawn* HitCharacter, FVector HitLocation,
                                       UHealthComponent* HealthComponent) const
{
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyeCharacter* Player = Cast<ABlindEyeCharacter>(Owner))
		{
			DamageInterface->Execute_TryApplyMarker(HealthComponent, Player->PlayerType);
		}
	}
}
