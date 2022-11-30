// Copyright (C) Nicholas Johnson 2022


#include "HUD/HunterHealthbar.h"

void UHunterHealthbar::SubscribeToHunter(AHunterEnemy* hunter)
{
	Hunter = MakeWeakObjectPtr(hunter);
	Hunter->HealthComponent->OnDamageDelegate.AddDynamic(this, &UHunterHealthbar::HunterHealthUpdated);
	UpdateHealth(Hunter->GetHealthPercent());
}

void UHunterHealthbar::HunterHealthUpdated(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{
	if (Hunter.IsValid())
	{
		UpdateHealth(Hunter->GetHealthPercent());
	}
}
