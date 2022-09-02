// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

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
	void Stun();
 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void KnockBack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Burn();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Stagger();
};
