// Copyright (C) Nicholas Johnson 2022

#include "Shrine.h"

#include "Components/HealthComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShrine::AShrine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	RootComponent = CapsuleComponent;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

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

void AShrine::OnDeath_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(world);
	if (ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(GameMode))
	{
		BlindEyeGameMode->OnShrineDeath();
	}
}

bool AShrine::GetIsDead_Implementation()
{
	return CurrShrineHealth <= 0;
}

UHealthComponent* AShrine::GetHealthComponent_Implementation()
{
	return HealthComponent;
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

