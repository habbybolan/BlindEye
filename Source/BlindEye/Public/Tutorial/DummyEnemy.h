// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "DummyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ADummyEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()

public:
	ADummyEnemy(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	float DeathDelay = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float AnimDuration = 1.f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EEasingFunc::Type> SpawningEaseFunc = EEasingFunc::Linear;
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EEasingFunc::Type> DespawnngEaseFunc = EEasingFunc::Linear;
	
	void KillDummy();

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* DummyMesh;

	virtual void BeginPlay() override;

protected:
	FTimerHandle DeathTimerHandle;
	FTimerHandle AnimTimerHandle;

	FVector StartPos;
	FVector EndPos;

	bool bSPawning = true;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_KilLDummyHelper();

	UFUNCTION()
	void CustomDeath();

	UFUNCTION()
	void SpawnDeathMovement();
	float CurrAnimDuration = 0;
	float AnimDelay = 0.02;
	
};
