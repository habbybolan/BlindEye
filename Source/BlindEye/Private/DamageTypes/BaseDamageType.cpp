// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/BaseDamageType.h"
#include "Components/HealthComponent.h"
#include "DamageTypes/BaseStatusEffect.h"
#include "DamageTypes/DetonatorStatusEffect.h"
#include "DamageTypes/MarkerStatusEffect.h"
#include "Interfaces/HealthInterface.h"

UBaseDamageType::UBaseDamageType()
{
}

UBaseDamageType::~UBaseDamageType()
{
}

float UBaseDamageType::ProcessDamage(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const
{
	// Get team of instigator and hit pawn
	TEAMS HitTeam; 
	TEAMS InstigatorTeam; 
	
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Owner->GetInstigator()))
	{
		HitTeam = HealthInterface->GetTeam();
	} else return 0;

	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitCharacter))
	{
		InstigatorTeam = HealthInterface->GetTeam();
	} else return 0;
	
	for (TSubclassOf<UBaseStatusEffect> statusEffectType : StatusEffects)
	{
		UBaseStatusEffect* statusEffect = NewObject<UBaseStatusEffect>(GetTransientPackage(), statusEffectType);
		// only allow Marker or detonator status effect if applied to same team (Players can mark/detonate each other)
		if (!Cast<UMarkerStatusEffect>(statusEffect) &&
			!Cast<UDetonatorStatusEffect>(statusEffect))
		{
			if (HitTeam == InstigatorTeam) continue;
		}

		// apply status effect
		// TODO: Delete pointer?
		//TUniquePtr<UBaseStatusEffect> statusEffect = TUniquePtr<UBaseStatusEffect>(NewObject<UBaseStatusEffect>(GetTransientPackage(), statusEffectType));
		statusEffect->ProcessEffect(Owner, HitCharacter, HitLocation, HealthComponent);
		//delete statusEffect;
	}
	// dont apply any damage if same team
	if (DebugDamageEverything == false && HitTeam == InstigatorTeam) return 0;
	return DamageMultiplier;
}
