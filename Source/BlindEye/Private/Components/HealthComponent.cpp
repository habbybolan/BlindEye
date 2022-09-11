// Copyright (C) Nicholas Johnson 2022


#include "Components/HealthComponent.h"

#include "DamageTypes/BaseDamageType.h"
#include "Interfaces/HealthInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	IsInvincible = false; // make sure this debug starts false
}


const FAppliedStatusEffects& UHealthComponent::GetAppliedStatusEffect()
{
	return AppliedStatusEffects;
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
		 
		float damageMultiplied = Damage * baseDamageType->ProcessDamage(DamageCauser, GetOwner(), HitLocation, this);

		// Debug invincibility
		if (IsInvincible) Damage = 0;
		
		OwnerHealth->Execute_SetHealth(GetOwner(), OwnerHealth->Execute_GetHealth(GetOwner()) - damageMultiplied);
		// send callback to owning actor for any additional logic
		OwnerHealth->Execute_OnTakeDamage(GetOwner(), Damage, HitLocation, DamageType, DamageCauser->GetInstigator());
		
		if (OwnerHealth->Execute_GetHealth(GetOwner()) <= 0)
		{
			// TODO: Check if player character or enemy
			// TODO: If enemy delete, if player, do extra work on player and send to GameMode for any state change
			OnDeathDelegate.Broadcast(GetOwner());
			OwnerHealth->Execute_OnDeath(GetOwner());
		}
	}
}


void UHealthComponent::Stun_Implementation(float StunDuration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Stunned");
	// TODO: Only applies to enemy
	//		Pause brain logic and play animation?
}

void UHealthComponent::KnockBack_Implementation(FVector KnockBackForce)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Knockback");
	/* TODO:
	* If player
	*	set player state as stunned and knock away from HitPoint
	* if enemy
	*	...
	*/
}

void UHealthComponent::Burn_Implementation(float DamagePerSec, float Duration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Burn");
	// TODO:
	// only effects enemy, damage over time
}

void UHealthComponent::Stagger_Implementation()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Stagger");
	// TODO: probably call stun?
	// ...
}

void UHealthComponent::TryApplyMarker_Implementation(PlayerType Player)
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	if (CurrMark != nullptr)
	{
		// TODO: Add Mark visual
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, "Already Marked");

		// Refresh the Mark if same mark used on enemy
		float TimeRemaining = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(world, MarkerDecayTimerHandle);
		float RefreshedTime = UKismetMathLibrary::Min(TimeRemaining + RefreshMarkerAmount, MarkerDecay);
		world->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
		world->GetTimerManager().SetTimer(MarkerDecayTimerHandle, this, &UHealthComponent::RemoveMark, RefreshedTime, false);
	} else
	{
		// Set the decay timer on marker
		world->GetTimerManager().SetTimer(MarkerDecayTimerHandle, this, &UHealthComponent::RemoveMark, MarkerDecay, false);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, "Mark Applied");
		CurrMark = new FMarkData();
		CurrMark->InitializeData(Player);
	}
}

void UHealthComponent::TryDetonation_Implementation(PlayerType Player)
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	if (CurrMark != nullptr)
	{
		// TODO: perform marker effect and remove marker

		// Detonate mark if of different type, clear decay timer
		if (CurrMark->MarkPlayerType != Player)
		{
			world->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, "Marker detonated on: " + GetOwner()->GetName());
			RemoveMark();
		}
	}
}

void UHealthComponent::TryTaunt_Implementation(float Duration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Taunt");
	// TODO: 
}

void UHealthComponent::RemoveMark()
{
	// TODO: Remove Mark visual
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Orange, "Marker removed");
	CurrMark = nullptr;
}

void UHealthComponent::OnRep_IsInvincibility()
{
	FString boolString = IsInvincible ? "True" : "False";
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Orange, "Invincibility: " + boolString);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, IsInvincible);
}
