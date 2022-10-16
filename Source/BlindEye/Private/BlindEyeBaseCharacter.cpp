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

ECharacterTypes ABlindEyeBaseCharacter::GetCharacterType(AActor* Character)
{
	if (Character == nullptr) return ECharacterTypes::Other;

	if (const ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(Character))
	{
		if (PlayerCharacter->PlayerType == EPlayerType::CrowPlayer)
		{
			return ECharacterTypes::Crow;
		} else
		{
			return ECharacterTypes::Phoenix;
		}
	} else if (const ASnapperEnemy* SnapperEnemy = Cast<ASnapperEnemy>(Character))
	{
		return ECharacterTypes::Snapper;
	} else if (const ABurrowerEnemy* BurrowerEnemy = Cast<ABurrowerEnemy>(Character))
	{
		return ECharacterTypes::Burrower;
	} else if (const AHunterEnemy* HunterEnemy = Cast<AHunterEnemy>(Character))
	{
		return ECharacterTypes::Hunter;
	} 
	return ECharacterTypes::Other;
}

float ABlindEyeBaseCharacter::GetMass()
{
	return Mass;
}

void ABlindEyeBaseCharacter::OnMarkAdded(EPlayerType MarkType)
{
	BP_OnMarkAdded(MarkType);
	MULT_OnMarkAddedHelper(MarkType);
}

void ABlindEyeBaseCharacter::MULT_OnMarkAddedHelper_Implementation(EPlayerType MarkerType)
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

