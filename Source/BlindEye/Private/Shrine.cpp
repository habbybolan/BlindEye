// Copyright (C) Nicholas Johnson 2022

#include "Shrine.h"

#include "Components/HealthComponent.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AShrine::AShrine()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	RootComponent = Mesh;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(CapsuleComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	CurrShrineHealth = MaxShrineHealth;
}


void AShrine::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		UWorld* World = GetWorld();
		check(World)
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		BlindEyeGS->GameStartedDelegate.AddDynamic(this, &AShrine::MULT_OnGameStarted);
	}
}

void AShrine::MULT_OnGameStarted_Implementation()
{
	UWorld* World = GetWorld();
	check(World)
	World->GetTimerManager().SetTimer(ChargeUpdatingTimerHandle, this, &AShrine::UpdateChargeUI, ChargeUpdatingDelay, true);
}

void AShrine::UpdateChargeUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		BP_UpdateShrineCharge(BlindEyeGS->GetGameDonePercent());
	}
	
}

float AShrine::GetMass()
{
	return 0;
}

float AShrine::GetHealth()
{
	return CurrShrineHealth;
}

void AShrine::SetHealth(float NewHealth)
{
	CurrShrineHealth = NewHealth;
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_HealthUpdated();
	}
}

TEAMS AShrine::GetTeam()
{
	return TEAMS::Player;
}

float AShrine::GetHealthPercent()
{
	return CurrShrineHealth / MaxShrineHealth;
}

void AShrine::OnDeath(AActor* ActorThatKilled)
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(world);
	if (ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(GameMode))
	{
		BlindEyeGameMode->OnShrineDeath();
	}
}

bool AShrine::GetIsDead()
{
	return CurrShrineHealth <= 0;
}

UHealthComponent* AShrine::GetHealthComponent()
{
	return HealthComponent;
}

void AShrine::ChannelingStarted(ABlindEyeEnemyBase* EnemyChannelling)
{
	if (!EnemiesCurrentlyChanneling.Contains(EnemyChannelling))
	{
		EnemiesCurrentlyChanneling.Add(EnemyChannelling);
		EnemyChannelling->GetHealthComponent()->OnDeathDelegate.AddDynamic(this, &AShrine::ChannellingEnded);
	}
}

void AShrine::ChannellingEnded(AActor* EnemyChannelling)
{
	// remove enemy delegate and from set of channelling enemies
	ABlindEyeEnemyBase* Enemy = Cast<ABlindEyeEnemyBase>(EnemyChannelling);
	if (ensure(Enemy))
	{
		if (EnemiesCurrentlyChanneling.Contains(Enemy))
		{
			Enemy->GetHealthComponent()->OnDeathDelegate.Remove(this, TEXT("ChannellingEnded"));
			EnemiesCurrentlyChanneling.Remove(Enemy);
		}
	}
}

FVector AShrine::GetIndicatorPosition()
{
	return Mesh->GetComponentLocation() + FVector::UpVector * 200.f;
}

void AShrine::OnRep_HealthUpdated()
{
	ShrineHealthChange.Broadcast();
} 

void AShrine::OnChannellingEnemyDied(AActor* DeadChannellingEnemy)
{
	ChannellingEnded(DeadChannellingEnemy);
}

void AShrine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShrine, CurrShrineHealth);
}

float AShrine::GetMaxHealth()
{
	return MaxShrineHealth;
}

void AShrine::MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser)
{
	BP_OnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);
}

UMarkerComponent* AShrine::GetMarkerComponent()
{
	return nullptr;
}

