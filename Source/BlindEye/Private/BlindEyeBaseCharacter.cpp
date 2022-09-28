// Copyright (C) Nicholas Johnson 2022


#include "BlindEyeBaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"

// Sets default values
ABlindEyeBaseCharacter::ABlindEyeBaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>("MarkerComponent");
	MarkerComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ABlindEyeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->MarkedAddedDelegate.AddUFunction(this, TEXT("OnMarkAdded"));
	HealthComponent->MarkedRemovedDelegate.AddUFunction(this, TEXT("OnMarkRemoved"));
	HealthComponent->DetonateDelegate.AddUFunction(this, TEXT("OnMarkDetonated"));
}

void ABlindEyeBaseCharacter::OnDeath_Implementation(AActor* ActorThatKilled)
{
	MULT_OnDeathHelper(ActorThatKilled);
}

void ABlindEyeBaseCharacter::MULT_OnDeathHelper_Implementation(AActor* ActorThatKilled)
{
	BP_OnDeath(ActorThatKilled);
}

TEAMS ABlindEyeBaseCharacter::GetTeam_Implementation()
{
	return Team;
}

void ABlindEyeBaseCharacter::MULT_OnTakeDamageHelper_Implementation(float Damage, FVector HitLocation,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	BP_OnTakeDamage(Damage, HitLocation, DamageType, DamageCauser);
}

void ABlindEyeBaseCharacter::MYOnTakeDamage_Implementation(float Damage, FVector HitLocation,
                                                           const UDamageType* DamageType, AActor* DamageCauser)
{
	MULT_OnTakeDamageHelper(Damage, HitLocation, DamageType, DamageCauser);
}

UHealthComponent* ABlindEyeBaseCharacter::GetHealthComponent_Implementation()
{
	return HealthComponent;
}

UMarkerComponent* ABlindEyeBaseCharacter::GetMarkerComponent_Implementation()
{
	return MarkerComponent;
}

void ABlindEyeBaseCharacter::OnMarkAdded(PlayerType MarkType)
{ 
	MULT_OnMarkAddedHelper(MarkType);
}

void ABlindEyeBaseCharacter::MULT_OnMarkAddedHelper_Implementation(PlayerType MarkerType)
{
	BP_OnMarkAdded(MarkerType);
	MarkerComponent->AddMark(MarkerType);
}

void ABlindEyeBaseCharacter::OnMarkRemoved()
{
	MULT_OnMarkRemovedHelper();
}

void ABlindEyeBaseCharacter::MULT_OnMarkRemovedHelper_Implementation()
{
	BP_OnMarkRemoved();
	MarkerComponent->RemoveMark();
}

void ABlindEyeBaseCharacter::OnMarkDetonated()
{
	MULT_OnMarkDetonatedHelper();
}

void ABlindEyeBaseCharacter::MULT_OnMarkDetonatedHelper_Implementation()
{
	BP_OnMarkDetonated();
	MarkerComponent->DetonateMark();
}

