// Copyright (C) Nicholas Johnson 2022


#include "Components/HealthComponent.h"

#include "DamageTypes/BaseDamageType.h"
#include "Interfaces/HealthInterface.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerHealth = Cast<IHealthInterface>(GetOwner());

	GetOwner()->OnTakePointDamage.AddDynamic(this, &UHealthComponent::SetPointDamage);
	GetOwner()->OnTakeRadialDamage.AddDynamic(this, &UHealthComponent::SetRadialDamage);
}

void UHealthComponent::SetPointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	SetDamage(Damage, HitLocation, DamageType, DamageCauser);
}

void UHealthComponent::SetRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	SetDamage(Damage, HitInfo.Location, DamageType, DamageCauser);
}

void UHealthComponent::SetDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (OwnerHealth && GetOwnerRole() == ROLE_Authority)
	{
		const UBaseDamageType* baseDamageType = Cast<UBaseDamageType>(DamageType);
		if (!baseDamageType) return;

		APawn* pawn = Cast<APawn>(GetOwner());
		if (!pawn) return;
		
		float damageMultiplied = Damage * baseDamageType->ProcessDamage(DamageCauser, pawn, HitLocation, this);
		OwnerHealth->Execute_SetHealth(GetOwner(), OwnerHealth->Execute_GetHealth(GetOwner()) - damageMultiplied);

		if (OwnerHealth->Execute_GetHealth(GetOwner()) <= 0)
		{
			// TODO: Check if player character or enemy
			// TODO: If enemy delete, if player, do extra work on player and send to GameMode for any state change
		}
	}
}


void UHealthComponent::Stun_Implementation(float StunDuration)
{
	// TODO: Only applies to enemy
	//		Pause brain logic and play animation?
}

void UHealthComponent::KnockBack_Implementation(FVector KnockBackForce)
{
	/* TODO:
	* If player
	*	set player state as stunned and knock away from HitPoint
	* if enemy
	*	...
	*/
}

void UHealthComponent::Burn_Implementation(float DamagePerSec, float Duration)
{
	// TODO:
	// only effects enemy, damage over time
}

void UHealthComponent::Stagger_Implementation()
{
	// TODO: probably call stun?
	// ...
}

void UHealthComponent::TryApplyMarker_Implementation(PlayerType Player, uint8 UniqueAbilityIndexMarker)
{
	// TODO: Add Mark visual
	CurrMark = new FMarkData();
	CurrMark->InitializeData(Player, UniqueAbilityIndexMarker);
}

void UHealthComponent::TryDetonation_Implementation(PlayerType Player, uint8 UniqueAbilityIndexMarker)
{
	if (CurrMark != nullptr)
	{
		// TODO: perform marker effect and remove marker
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Orange, "Marker detonated on: " + GetOwner()->GetName());
		RemoveMark();
	}
}

void UHealthComponent::RemoveMark()
{
	// TODO: Remove Mark visual
	CurrMark = nullptr;
}

