// Copyright (C) Nicholas Johnson 2022


#include "Enemies/SnapperEnemy.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/SnapperEnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ASnapperEnemy::ASnapperEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void ASnapperEnemy::MYOnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
                                                AActor* DamageCauser)
{
	Super::MYOnTakeDamage_Implementation(Damage, HitLocation, DamageType, DamageCauser);

	// If taken damage from player, set Target as player
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(DamageCauser))
	{
		if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
		{
			SnapperController->SetTargetEnemy(BlindEyeCharacter);
		}
	}
}

void ASnapperEnemy::PerformBasicAttack()
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
