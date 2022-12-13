// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemy.h"
#include "MarkingDummyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API AMarkingDummyEnemy : public ADummyEnemy
{
	GENERATED_BODY()

public:

	AMarkingDummyEnemy(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	float DelayRemovingMark = 2.f;

	virtual void OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType) override;

protected:
};
