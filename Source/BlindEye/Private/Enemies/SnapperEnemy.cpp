// Copyright (C) Nicholas Johnson 2022


#include "Enemies/SnapperEnemy.h"
#include "Characters/BlindEyeCharacter.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/SnapperEnemyController.h"

void ASnapperEnemy::OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{
	Super::OnTakeDamage_Implementation(Damage, HitLocation, DamageType, DamageCauser);

	// If taken damage from player, set Target as player
	if (ABlindEyeCharacter* BlindEyeCharacter = Cast<ABlindEyeCharacter>(DamageCauser))
	{
		if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(GetController()))
		{
			SnapperController->SetTargetEnemy(BlindEyeCharacter);
		}
	}
}

void ASnapperEnemy::PerformBasicAttack()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Red, "Try Attack");
}
