// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemy.h"
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

void AHunterEnemy::TrySetVisibility(bool visibility)
{
	if (IsVisible == visibility) return;

	IsVisible = visibility;
	MULT_TurnVisible(visibility);
}

void AHunterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: Move to timer for optimization
	//	make run only on server
	 // if (IsVisible == false)
	 // {
	 // 	UWorld* World = GetWorld();
	 // 	if (World == nullptr) return;
	 //
	 // 	TArray<AActor*> OutActors;
	 // 	if (UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), RadiusToTurnVisible,
	 // 		ObjectTypes, nullptr, TArray<AActor*>(), OutActors))
	 // 	{
	 // 		// Player close, turn visible
	 // 		TrySetVisibility(true);
	 // 	}
	 // }
	
}

void AHunterEnemy::MULT_TurnVisible_Implementation(bool visibility)
{
	TrySetVisibiltiyHelper(visibility);
}

