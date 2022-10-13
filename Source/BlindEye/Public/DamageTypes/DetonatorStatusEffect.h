// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "DetonatorStatusEffect.generated.h"

enum class EPlayerType : uint8;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UDetonatorStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerType DetonatorType;
	
	virtual void ProcessEffect(AActor* Owner, AActor* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
};
