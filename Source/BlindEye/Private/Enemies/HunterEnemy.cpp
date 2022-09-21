// Copyright (C) Nicholas Johnson 2022


#include "Enemies/HunterEnemy.h"
#include "DamageTypes/BaseDamageType.h"
#include "Kismet/GameplayStatics.h"

AHunterEnemy::AHunterEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void AHunterEnemy::PerformBasicAttack()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 300, FVector(0, 100 / 2, 100 / 2),
		GetActorRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);

	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		UGameplayStatics::ApplyPointDamage(HitActor, BasicAttackDamage, Hit.ImpactNormal, Hit, GetController(), this, BasicAttackDamageType);
	}
}
