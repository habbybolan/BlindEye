// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/MarkingTutorial/MarkingDummyEnemy.h"

#include "Tutorial/DummyEnemyHealthComponent.h"
#include "Tutorial/MarkingTutorial/MarkingDummyHealthComponent.h"

AMarkingDummyEnemy::AMarkingDummyEnemy(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UMarkingDummyHealthComponent>(TEXT("HealthComponent")))
{
}

void AMarkingDummyEnemy::OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType)
{
	Super::OnMarkAdded(MarkedActor, MarkerType);
}
