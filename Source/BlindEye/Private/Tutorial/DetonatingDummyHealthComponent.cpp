// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DetonatingDummyHealthComponent.h"

UDetonatingDummyHealthComponent::UDetonatingDummyHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void UDetonatingDummyHealthComponent::RemoveMark()
{
	// Do nothing
}

void UDetonatingDummyHealthComponent::TryApplyMarker(EMarkerType Player, AActor* DamageCause)
{
	Super::TryApplyMarker(Player, DamageCause);
}
