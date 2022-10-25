// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterHealthComponent.h"

#include "Enemies/Hunter/HunterEnemy.h"
#include "Enemies/Hunter/HunterEnemyController.h"

void UHunterHealthComponent::KnockBack(FVector KnockBackForce, AActor* DamageCause)
{
	// Prevent Knockback
}

float UHunterHealthComponent::ProcessDamage(float Damage)
{
	AHunterEnemy* Hunter = Cast<AHunterEnemy>(GetOwner());
	check(Hunter);

	// Alter damage intake if hunter not charged
	if (!Hunter->GetIsCharged())
	{
		return Damage * Hunter->UnchargedDamagePercent;
	}
	return Damage;
}
