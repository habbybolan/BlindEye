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
	HealthComponent->StunStartDelegate.AddUFunction(this, TEXT("OnStunStart"));
	HealthComponent->StunEndDelegate.AddUFunction(this, TEXT("OnStunEnd"));
	HealthComponent->BurnDelegateStart.AddUFunction(this, TEXT("OnBurnStart"));
	HealthComponent->BurnDelegateEnd.AddUFunction(this, TEXT("OnBurnEnd"));
	HealthComponent->TauntStartDelegate.AddUFunction(this, TEXT("OnTauntStart"));
	HealthComponent->TauntEndDelegate.AddUFunction(this, TEXT("OnTauntEnd"));
}

void ABlindEyeEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyeEnemyBase, CurrHealth);
}

void ABlindEyeEnemyBase::OnStunStart(float StunDuration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Enemy Stunned: " + FString::SanitizeFloat(StunDuration));
}

void ABlindEyeEnemyBase::OnStunEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Stun ended");
}

void ABlindEyeEnemyBase::OnBurnStart(float DamagePerSec, float Duration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Burn Started");
}

void ABlindEyeEnemyBase::OnBurnEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Burn ended");
}

void ABlindEyeEnemyBase::OnTauntStart(float Duration, AActor* Taunter)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Taunt started for " + FString::SanitizeFloat(Duration) + "from: " + Taunter->GetName());
}

void ABlindEyeEnemyBase::OnTauntEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Taunt ended");
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

