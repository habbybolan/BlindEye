// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemyHealthComponent.h"
#include "DetonatingDummyHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UDetonatingDummyHealthComponent : public UDummyEnemyHealthComponent
{
	GENERATED_BODY()

public:
	explicit UDetonatingDummyHealthComponent(const FObjectInitializer& ObjectInitializer);

	virtual void RemoveMark() override;

	virtual void TryApplyMarker(EMarkerType Player, AActor* DamageCause) override;
};
