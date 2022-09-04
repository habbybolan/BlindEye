// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

enum class PlayerType : uint8;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Stun(float StunDuration);
 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void KnockBack(FVector KnockBackForce);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Burn(float DamagePerSec, float Duration);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Stagger();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TryApplyMarker(PlayerType Player, uint8 UniqueAbilityIndexMarker);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TryDetonation(PlayerType Player, uint8 UniqueAbilityIndexMarker);
};
