// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "DetonatorDamageType.generated.h"

enum class PlayerType : uint8;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UDetonatorDamageType : public UBaseDamageType
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	PlayerType DetonatorType;
	
	virtual float ProcessDamage(AActor* Owner, APawn* HitCharacter, FVector HitLocation, UHealthComponent* HealthComponent) const override;
	
};
