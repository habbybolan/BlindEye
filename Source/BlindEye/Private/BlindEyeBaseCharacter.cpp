// Copyright (C) Nicholas Johnson 2022


#include "BlindEyeBaseCharacter.h"

#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"

// Sets default values
ABlindEyeBaseCharacter::ABlindEyeBaseCharacter()
{
	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>("MarkerComponent");
	MarkerComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ABlindEyeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->MarkedAddedDelegate.AddUFunction(this, TEXT("OnMarkAdded"));
	HealthComponent->MarkedRemovedDelegate.AddUFunction(this, TEXT("OnMarkRemoved"));
	HealthComponent->DetonateDelegate.AddUFunction(this, TEXT("OnMarkDetonated"));
}

TEAMS ABlindEyeBaseCharacter::GetTeam_Implementation()
{
	return Team;
}

UHealthComponent* ABlindEyeBaseCharacter::GetHealthComponent_Implementation()
{
	return HealthComponent;
}

UMarkerComponent* ABlindEyeBaseCharacter::GetMarkerComponent_Implementation()
{
	return MarkerComponent;
}

void ABlindEyeBaseCharacter::OnMarkAdded()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Mark added");
}

void ABlindEyeBaseCharacter::OnMarkRemoved()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Mark removed");
}

void ABlindEyeBaseCharacter::OnMarkDetonated()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Blue, "Mark detonated");
}

