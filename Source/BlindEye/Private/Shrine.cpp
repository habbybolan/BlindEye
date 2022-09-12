// Copyright (C) Nicholas Johnson 2022

#include "Shrine.h"

#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShrine::AShrine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	CurrShrineHealth = MaxShrineHealth;
}

float AShrine::GetHealth_Implementation()
{
	return CurrShrineHealth;
}

void AShrine::SetHealth_Implementation(float NewHealth)
{
	CurrShrineHealth = NewHealth;
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_HealthUpdated();
	}
}

TEAMS AShrine::GetTeam_Implementation()
{
	return TEAMS::Player;
}

float AShrine::GetHealthPercent_Implementation()
{
	return CurrShrineHealth / MaxShrineHealth;
}

// Called when the game starts or when spawned
void AShrine::BeginPlay()
{
	Super::BeginPlay();
}

void AShrine::OnRep_HealthUpdated()
{
	ShrineHealthChange.Broadcast();
}

void AShrine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShrine, CurrShrineHealth);
}
