// Copyright (C) Nicholas Johnson 2022


#include "BlindEyeBaseCharacter.h"

#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"

// Sets default values
ABlindEyeBaseCharacter::ABlindEyeBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>("MarkerComponent");
	MarkerComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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

// Called when the game starts or when spawned
void ABlindEyeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlindEyeBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlindEyeBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

