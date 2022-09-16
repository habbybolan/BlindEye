// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkerStatusEffect.h"

#include "Components/HealthComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Interfaces/DamageInterface.h"

void UMarkerStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                       UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		if (const ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Owner->GetInstigator()))
		{
			DamageInterface->Execute_TryApplyMarker(HealthComponent, Player->PlayerType, Owner);
		}
	}
}
