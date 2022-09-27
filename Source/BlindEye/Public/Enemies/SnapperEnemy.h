// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "SnapperEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASnapperEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:
	ASnapperEnemy(const FObjectInitializer& ObjectInitializer);
	
	virtual void MYOnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BasicAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> BasicAttackDamageType;

	void PerformBasicAttack();

	void TryRagdoll(bool SimulatePhysics);

	void TempLaunch();
	void LaunchSwing();

protected:

	bool bRagdolling = false;
	FTimerHandle LaunchSwingTimerHandle;
	FTimerHandle ColliderOnMeshTimerHandle;

	void TeleportColliderToMesh();

	void StartRagdoll();
	void StopRagdoll();
	
};
