// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperHealthComponent.h"

#include "Enemies/Snapper/SnapperEnemy.h"

void USnapperHealthComponent::KnockBack(FVector KnockBackForce, AActor* DamageCause)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// TODO: Only apply ragdoll delay to crow flurry?
	if (!bCanKnockBack) return;

	bCanKnockBack = false;
	World->GetTimerManager().SetTimer(KnockBackCooldownTimer, this, &USnapperHealthComponent::KnockBackOffCooldown, KnockBackCooldown, false);
	
	if (ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(GetOwner()))
	{
		Snapper->ApplyKnockBack(KnockBackForce);
	}
}

void USnapperHealthComponent::KnockBackOffCooldown()
{
	bCanKnockBack = true;
}
