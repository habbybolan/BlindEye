// Copyright (C) Nicholas Johnson 2022

#include "Enemies/BlindEyeEnemybase.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"
#include "Net/UnrealNetwork.h"

ABlindEyeEnemyBase::ABlindEyeEnemyBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ABlindEyeEnemyBase::MYOnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{}

void ABlindEyeEnemyBase::OnDeath_Implementation()
{
	UnPossessed();
	Destroy();
}

float ABlindEyeEnemyBase::GetHealthPercent_Implementation()
{
	return CurrHealth / MaxHealth;
}

float ABlindEyeEnemyBase::GetMaxHealth_Implementation()
{
	return MaxHealth;
}
