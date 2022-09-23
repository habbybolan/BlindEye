// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "HunterEnemy.generated.h"

class UBaseDamageType;

/**
 * 
 */
UCLASS()
class BLINDEYE_API AHunterEnemy : public ABlindEyeEnemyBase
{
	GENERATED_BODY()
	
public:

	AHunterEnemy(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BasicAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> BasicAttackDamageType;

	UPROPERTY(BlueprintReadWrite)
	bool IsVisible = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadiusToTurnVisible = 500;

	void PerformBasicAttack();

	void TrySetVisibility(bool visibility);
 
protected:
 
	virtual void Tick(float DeltaSeconds) override;

	// Intermediary method to make RPC call to blueprint implementable method
	UFUNCTION(NetMulticast, Reliable)
	void MULT_TurnVisible(bool visibility);
 
	UFUNCTION(BlueprintImplementableEvent)
	void TrySetVisibiltiyHelper(bool visibility);
	
};
