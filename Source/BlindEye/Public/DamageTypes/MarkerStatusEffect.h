// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusEffect.h"
#include "MarkerStatusEffect.generated.h"

enum class PlayerType : uint8;

/**
 * 
 */
UCLASS()
class BLINDEYE_API UMarkerStatusEffect : public UBaseStatusEffect
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	PlayerType MarkerType;
	
	virtual void ProcessEffect(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
