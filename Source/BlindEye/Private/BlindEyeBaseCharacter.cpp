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

void ABlindEyeBaseCharacter::OnMarkAdded(PlayerType MarkType)
{ 
	MarkerComponent->AddMark(MarkType);
}

void ABlindEyeBaseCharacter::OnMarkRemoved()
{
	MarkerComponent->RemoveMark();
}

void ABlindEyeBaseCharacter::OnMarkDetonated()
{
	MarkerComponent->DetonateMark();
}

