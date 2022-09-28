// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperHealthComponent.h"

#include "Enemies/Snapper/SnapperEnemy.h"

void USnapperHealthComponent::KnockBack(FVector KnockBackForce, AActor* DamageCause)
{
	Super::KnockBack(KnockBackForce, DamageCause);

	if (ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(GetOwner()))
	{
		Snapper->TryRagdoll(true);
	}
}
