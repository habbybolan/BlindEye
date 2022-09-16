// Copyright (C) Nicholas Johnson 2022


#include "Components/HealthComponent.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "DamageTypes/BaseDamageType.h"
#include "DamageTypes/BaseStatusEffect.h"
#include "DamageTypes/StunStatusEffect.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/HealthInterface.h"
#include "Kismet/GameplayStatics.h"
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
		if (IsInvincible) damageMultiplied = 0;
		
		OwnerHealth->Execute_SetHealth(GetOwner(), OwnerHealth->Execute_GetHealth(GetOwner()) - damageMultiplied);
		// send callback to owning actor for any additional logic
		OwnerHealth->Execute_OnTakeDamage(GetOwner(), Damage, HitLocation, DamageType, DamageCauser->GetInstigator());
		
		if (OwnerHealth->Execute_GetHealth(GetOwner()) <= 0)
		{
			// TODO: Check if player character or enemy
			// TODO: If enemy delete, if player, do extra work on player and send to GameMode for any state change
			OnDeath();
		}
	}
}

void UHealthComponent::OnDeath()
{
	OnDeathDelegate.Broadcast(GetOwner());
	OwnerHealth->Execute_OnDeath(GetOwner());
}

void UHealthComponent::Stun_Implementation(float StunDuration, AActor* DamageCause)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AppliedStatusEffects.IsStun)
	{
		float RemainingTimeOnStun = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(World, StunTimerHandle);
		// Do nothing if trying to override with less stun duration
		if (StunDuration <= RemainingTimeOnStun)
		{
			return;
		}
	} 
	World->GetTimerManager().SetTimer(StunTimerHandle, this, &UHealthComponent::RemoveStun, StunDuration, false);
	AppliedStatusEffects.IsStun = true;
	StunStartDelegate.Broadcast(StunDuration);
}

void UHealthComponent::KnockBack_Implementation(FVector KnockBackForce, AActor* DamageCause)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->AddImpulse(KnockBackForce * 100);
		KnockBackDelegate.Broadcast(KnockBackForce);
	}
}

void UHealthComponent::Burn_Implementation(float DamagePerSec, float Duration, AActor* DamageCause)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AppliedStatusEffects.IsBurn)
	{
		float RemainingTimeOnStun = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(World, BurnTimerHandle);
		// Do nothing if trying to override with less stun duration
		if (Duration <= RemainingTimeOnStun)
		{
			return; 
		}
	} else
	{
		World->GetTimerManager().SetTimer(BurnAppliedTimerHandle, this, &UHealthComponent::ApplyBurn, DelayBetweenBurnTicks, true, 0);
	}
	
	World->GetTimerManager().SetTimer(BurnTimerHandle, this, &UHealthComponent::RemoveBurn, Duration, false);
	AppliedStatusEffects.IsBurn = true;
	AppliedStatusEffects.BurnDPS = DamagePerSec;
	BurnDelegateStart.Broadcast(DamagePerSec, Duration);
}

void UHealthComponent::Stagger_Implementation(AActor* DamageCause)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Silver, "Stagger");
	// TODO: probably call stun?
	// ...
}

void UHealthComponent::TryApplyMarker_Implementation(PlayerType Player, AActor* DamageCause)
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
		MarkedAddedDelegate.Broadcast(Player);
	}
}

void UHealthComponent::TryDetonation_Implementation(PlayerType Player, AActor* DamageCause)
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	if (CurrMark != nullptr)
	{
		// Detonate mark if of different type, clear decay timer
		if (CurrMark->MarkPlayerType != Player)
		{
			world->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, "Marker detonated on: " + GetOwner()->GetName());
			PerformDetonationEffect(DamageCause);
			DetonateMark();
		}
	}
}

void UHealthComponent::PerformDetonationEffect(AActor* DamageCause)
{
	// If enemy is being detonated
	if (ABlindEyeEnemyBase* BLindEyeEnemy = Cast<ABlindEyeEnemyBase>(GetOwner()))
	{
		if (CurrMark->MarkPlayerType == PlayerType::CrowPlayer)
		{
			UBaseDamageType* StunDamage = NewObject<UBaseDamageType>(GetTransientPackage(), DarkDetonationOnEnemyDamageType);
			SetDamage(DarkDetonationOnEnemyDamage, GetOwner()->GetActorLocation(), StunDamage, DamageCause);
		}
		else if (CurrMark->MarkPlayerType == PlayerType::PhoenixPlayer)
		{
			UBaseDamageType* BurnDamage = NewObject<UBaseDamageType>(GetTransientPackage(), FireDetonationOnEnemyDamageType);
			SetDamage(1, GetOwner()->GetActorLocation(), BurnDamage, DamageCause);
		}
	}
	else if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		if (CurrMark->MarkPlayerType == PlayerType::CrowPlayer)
		{
			UWorld* World = GetWorld();
			if (!World) return;
			
			UGameplayStatics::ApplyRadialDamage(World, DarkDetonationOnPlayerDamage, GetOwner()->GetActorLocation(),
				DarkDetonationOnPlayerRadius, DarkDetonationOnPlayerDamageType, TArray<AActor*>(), DamageCause);
			UBaseDamageType* ExplosionDamage = NewObject<UBaseDamageType>(GetTransientPackage(), DarkDetonationOnPlayerDamageType);
			SetDamage(1, GetOwner()->GetActorLocation(), ExplosionDamage, DamageCause);
		}
		else if (CurrMark->MarkPlayerType == PlayerType::PhoenixPlayer)
		{
			UWorld* World = GetWorld();
			if (!World) return;

			FActorSpawnParameters params;
			params.Instigator = Cast<APawn>(GetOwner());
			params.Owner = GetOwner();
			World->SpawnActor<AHealingWell>(HealingWellType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
		}
	}
}

void UHealthComponent::TryTaunt_Implementation(float Duration, AActor* Taunter)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AppliedStatusEffects.IsTaunt)
	{
		float RemainingTimeOnStun = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(World, TauntTimerHandle);
		// Do nothing if trying to override with less stun duration
		if (Duration <= RemainingTimeOnStun)
		{
			return;
		}
	} 
	World->GetTimerManager().SetTimer(TauntTimerHandle, this, &UHealthComponent::RemoveTaunt, Duration, false);
	AppliedStatusEffects.IsTaunt = true; 
	TauntStartDelegate.Broadcast(Duration, Taunter);
}

void UHealthComponent::RemoveMark()
{
	CurrMark = nullptr;
	MarkedRemovedDelegate.Broadcast();
}

void UHealthComponent::DetonateMark()
{
	CurrMark = nullptr;
	DetonateDelegate.Broadcast();
}

void UHealthComponent::OnRep_IsInvincibility()
{
	FString boolString = IsInvincible ? "True" : "False";
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Orange, "Invincibility: " + boolString);
}


void UHealthComponent::Kill()
{
	OnDeath();
}

void UHealthComponent::ImprovedHealing_Implementation(float HealPercentIncrease, float Duration)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AppliedStatusEffects.IsImprovedHealing)
	{
		float RemainingTimeOnHeal = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(World, ImprovedHealingTimerHandle);
		// Do nothing if trying to override with less stun duration
		if (Duration <= RemainingTimeOnHeal)
		{
			return;
		}
	} 
	World->GetTimerManager().SetTimer(ImprovedHealingTimerHandle, this, &UHealthComponent::RemoveImprovedHealing, Duration, false);
	AppliedStatusEffects.IsImprovedHealing = true; 
	AppliedStatusEffects.HealPercentIncrease = HealPercentIncrease; 
	ImprovedHealingStartDelegate.Broadcast(HealPercentIncrease, Duration);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, IsInvincible);
}

void UHealthComponent::RemoveStun()
{
	AppliedStatusEffects.IsStun = false;
	StunEndDelegate.Broadcast();
}

void UHealthComponent::RemoveBurn()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().ClearTimer(BurnAppliedTimerHandle);
	AppliedStatusEffects.IsBurn = false;
	BurnDelegateEnd.Broadcast();
}

void UHealthComponent::ApplyBurn()
{
	UBaseDamageType* DamageType =  NewObject<UBaseDamageType>();
	DamageType->DebugDamageEverything = true;
	// TODO: Apply damage based on damage cause, dont use self with debug param
	SetDamage(AppliedStatusEffects.BurnDPS * DelayBetweenBurnTicks,FVector::ZeroVector, DamageType, GetOwner());
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, DelayBetweenBurnTicks, FColor::Blue, "Burn: " + FString::SanitizeFloat(AppliedStatusEffects.BurnDPS * DelayBetweenBurnTicks));
}

void UHealthComponent::RemoveTaunt()
{
	AppliedStatusEffects.IsTaunt = false;
	TauntEndDelegate.Broadcast();
}

void UHealthComponent::RemoveImprovedHealing()
{
	AppliedStatusEffects.IsImprovedHealing = false;
	AppliedStatusEffects.HealPercentIncrease = 0; 
	ImprovedHealingEndDelegate.Broadcast();
}
