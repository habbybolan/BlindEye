// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/StunStatusEffect.h"
#include "Components/HealthComponent.h"
#include "Interfaces/DamageInterface.h"

void UStunStatusEffect::ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	if (Owner->GetInstigator() == HitCharacter) return;
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
	{
		DamageInterface->Stun(StunDuration, Owner);
	}
}
