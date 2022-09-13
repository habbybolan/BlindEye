// Copyright (C) Nicholas Johnson 2022

#include "Enemies/BlindEyeEnemybase.h"
#include "Characters/BlindEyeCharacter.h"
#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlindEyeEnemyBase::ABlindEyeEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	CurrHealth = MaxHealth;
	Team = TEAMS::Enemy;
}

const FAppliedStatusEffects& ABlindEyeEnemyBase::GetAppliedStatusEffects()
{
	return HealthComponent->GetAppliedStatusEffect();
}

// Called when the game starts or when spawned
void ABlindEyeEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlindEyeEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyeEnemyBase, CurrHealth);
}

// Called every frame
void ABlindEyeEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABlindEyeEnemyBase::GetHealth_Implementation()
{
	return CurrHealth;
}

void ABlindEyeEnemyBase::SetHealth_Implementation(float NewHealth)
{
	CurrHealth = NewHealth;
}

void ABlindEyeEnemyBase::OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{}

UHealthComponent* ABlindEyeEnemyBase::GetHealthComponent_Implementation()
{
	return HealthComponent;
}

void ABlindEyeEnemyBase::OnDeath_Implementation()
{
	UnPossessed();
	Destroy();
}

float ABlindEyeEnemyBase::GetHealthPercent_Implementation()
{
	return CurrHealth / MaxHealth;
}

TEAMS ABlindEyeEnemyBase::GetTeam_Implementation()
{
	return Team;
}

