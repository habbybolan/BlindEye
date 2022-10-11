// Copyright (C) Nicholas Johnson 2022


#include "BlindEyeBaseCharacter.h"

#include "Characters/CrowCharacter.h"
#include "Characters/PhoenixCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"
#include "Enemies/Hunter/HunterEnemy.h"

// Sets default values
ABlindEyeBaseCharacter::ABlindEyeBaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>("MarkerComponent");
	MarkerComponent->SetupAttachment(GetMesh());

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ABlindEyeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->MarkedAddedDelegate.AddUFunction(this, TEXT("OnMarkAdded"));
	HealthComponent->MarkedRemovedDelegate.AddUFunction(this, TEXT("OnMarkRemoved"));
	HealthComponent->DetonateDelegate.AddUFunction(this, TEXT("OnMarkDetonated"));
}

void ABlindEyeBaseCharacter::OnDeath(AActor* ActorThatKilled)
{
	BP_OnDeath(ActorThatKilled);
}

TEAMS ABlindEyeBaseCharacter::GetTeam()
{
	return Team;
}

void ABlindEyeBaseCharacter::MYOnTakeDamage(float Damage, FVector HitLocation,
                                                           const UDamageType* DamageType, AActor* DamageCauser)
{
	if (GetIsDead()) return;

	StoreCharacterTypeThatLastDamaged(DamageCauser);
	BP_OnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);
}

UHealthComponent* ABlindEyeBaseCharacter::GetHealthComponent()
{
	return HealthComponent;
}

UMarkerComponent* ABlindEyeBaseCharacter::GetMarkerComponent()
{
	return MarkerComponent;
}

void ABlindEyeBaseCharacter::StoreCharacterTypeThatLastDamaged(AActor* LastDamagedBy)
{
	if (const ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(LastDamagedBy))
	{
		if (PlayerCharacter->PlayerType == PlayerType::CrowPlayer)
		{
			LastDamageByType = ECharacterTypes::Crow;
		} else
		{
			LastDamageByType = ECharacterTypes::Phoenix;
		}
	} else if (const ASnapperEnemy* SnapperEnemy = Cast<ASnapperEnemy>(LastDamagedBy))
	{
		LastDamageByType = ECharacterTypes::Snapper;
	} else if (const ABurrowerEnemy* BurrowerEnemy = Cast<ABurrowerEnemy>(LastDamagedBy))
	{
		LastDamageByType = ECharacterTypes::Burrower;
	} else if (const AHunterEnemy* HunterEnemy = Cast<AHunterEnemy>(LastDamagedBy))
	{
		LastDamageByType = ECharacterTypes::Hunter;
	} 
}

ECharacterTypes ABlindEyeBaseCharacter::GetCharacterType()
{
	return LastDamageByType;
}

float ABlindEyeBaseCharacter::GetMass()
{
	return Mass;
}

void ABlindEyeBaseCharacter::OnMarkAdded(PlayerType MarkType)
{
	BP_OnMarkAdded(MarkType);
	MULT_OnMarkAddedHelper(MarkType);
}

void ABlindEyeBaseCharacter::MULT_OnMarkAddedHelper_Implementation(PlayerType MarkerType)
{
	MarkerComponent->AddMark(MarkerType);
}

void ABlindEyeBaseCharacter::OnMarkRemoved()
{
	BP_OnMarkRemoved();
	MULT_OnMarkRemovedHelper();
}

void ABlindEyeBaseCharacter::MULT_OnMarkRemovedHelper_Implementation()
{
	MarkerComponent->RemoveMark();
}

void ABlindEyeBaseCharacter::OnMarkDetonated()
{ 
	if (FMarkData* marker = HealthComponent->GetCurrMark())
	{
		BP_OnMarkDetonated(marker->MarkPlayerType);
		MULT_OnMarkDetonatedHelper();
	}
	
}

void ABlindEyeBaseCharacter::MULT_OnMarkDetonatedHelper_Implementation()
{
	MarkerComponent->DetonateMark();
}

float ABlindEyeBaseCharacter::GetHealth()
{
	return 0;
}

float ABlindEyeBaseCharacter::GetMaxHealth()
{
	return 0;
}

void ABlindEyeBaseCharacter::SetHealth(float NewHealth)
{}

float ABlindEyeBaseCharacter::GetHealthPercent()
{
	return 0;
}

bool ABlindEyeBaseCharacter::GetIsDead()
{
	return false;
}

