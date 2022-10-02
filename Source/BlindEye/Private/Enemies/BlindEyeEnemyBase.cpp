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

bool ABlindEyeEnemyBase::GetIsDead()
{
	return bIsDead;
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

float ABlindEyeEnemyBase::GetHealth()
{
	return CurrHealth;
}

void ABlindEyeEnemyBase::SetHealth(float NewHealth)
{
	CurrHealth = NewHealth;
}

void ABlindEyeEnemyBase::MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{
	Super::MYOnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);
}

void ABlindEyeEnemyBase::OnDeath(AActor* ActorThatKilled)
{
	Super::OnDeath(ActorThatKilled);
	bIsDead = true;
}

float ABlindEyeEnemyBase::GetHealthPercent()
{
	return CurrHealth / MaxHealth;
}

float ABlindEyeEnemyBase::GetMaxHealth()
{
	return MaxHealth;
}

void ABlindEyeEnemyBase::FellOutOfWorld(const UDamageType& dmgType)
{
	// reset player to a playerStart on killz reached
	if (GetLocalRole() == ROLE_Authority)
	{
		OnDeath(nullptr);
	}
}
