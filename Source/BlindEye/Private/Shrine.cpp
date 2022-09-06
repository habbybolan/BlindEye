// Copyright (C) Nicholas Johnson 2022

#include "Shrine.h"

#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShrine::AShrine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
}

float AShrine::GetHealth_Implementation()
{
	// TODO:
	return 0;
}

void AShrine::SetHealth_Implementation(float NewHealth)
{
	// TODO:
}

TEAMS AShrine::GetTeam_Implementation()
{
	return TEAMS::Player;
}

// Called when the game starts or when spawned
void AShrine::BeginPlay()
{
	Super::BeginPlay();
}

