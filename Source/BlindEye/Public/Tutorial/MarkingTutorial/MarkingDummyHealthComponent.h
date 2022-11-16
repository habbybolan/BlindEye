// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemyHealthComponent.h"
#include "MarkingDummyHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UMarkingDummyHealthComponent : public UDummyEnemyHealthComponent
{
	GENERATED_BODY()

public:
	
	virtual void TryDetonation(EPlayerType Player, AActor* DamageCause) override;
	
};
