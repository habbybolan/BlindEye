// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkerStatusEffect.h"

#include "Components/HealthComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Interfaces/DamageInterface.h"

void UMarkerStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation,
                                       UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		// Apply a player mark
		if (const ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Owner->GetInstigator()))
		{
			if (Player->PlayerType == EPlayerType::CrowPlayer)
			{
				DamageInterface->TryApplyMarker(EMarkerType::Crow, Owner);
			} else
			{
				DamageInterface->TryApplyMarker(EMarkerType::Phoenix, Owner);
			}
		}
		// Apply negative hunter mark
		else if (const AHunterEnemy* Hunter = Cast<AHunterEnemy>(Owner->GetInstigator()))
		{
			DamageInterface->TryApplyMarker(EMarkerType::Hunter, Owner);
		}
	}
}
