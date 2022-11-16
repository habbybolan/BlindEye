// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/MarkerComponent.h"
#include "UObject/Interface.h"
#include "BlindEyeUtils.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLINDEYE_API IDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void Stun(float StunDuration, AActor* DamageCause) = 0;
  
	UFUNCTION()
	virtual void KnockBack(FVector KnockBackForce, AActor* DamageCause) = 0;

	UFUNCTION()
	virtual void Burn(float DamagePerSec, float Duration, AActor* DamageCause) = 0;

	UFUNCTION()
	virtual void Stagger(AActor* DamageCause) = 0;

	UFUNCTION()
	virtual void TryApplyMarker(EMarkerType Player, AActor* DamageCause) = 0;

	UFUNCTION()
	virtual void TryDetonation(EPlayerType Player, AActor* DamageCause) = 0;
  
	UFUNCTION()
	virtual void TryTaunt(float Duration, AActor* Taunter) = 0;

	UFUNCTION()
	virtual void ImprovedHealing(float HealPercentIncrease, float Duration) = 0;
};
