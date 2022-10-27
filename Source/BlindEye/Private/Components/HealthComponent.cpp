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
UHealthComponent::UHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	// MUST SET REPLICATED TO TRUE IN BLUEPRINTS, CRASHES IF SET HERE
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

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().SetTimer(HealTimerHandle, this, &UHealthComponent::PerformHeal, PerformHealDelay, true);
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
		// prevent damage logic of already dead
		if (OwnerHealth->GetIsDead()) return;
			
		const UBaseDamageType* baseDamageType = Cast<UBaseDamageType>(DamageType);
		if (!baseDamageType) return;

		
		float damageMultiplied = Damage * baseDamageType->ProcessDamage(DamageCauser, GetOwner(), HitLocation, this);
		damageMultiplied = ProcessDamage(damageMultiplied);
		
		// Debug invincibility
		if (IsInvincible) damageMultiplied = 0;

		// Prevent calling any damage method if no damage applied
		if (damageMultiplied <= 0) return;
		
		OwnerHealth->SetHealth(OwnerHealth->GetHealth() - damageMultiplied);
		// send callback to owning actor for any additional logic
		OwnerHealth->MYOnTakeDamage(Damage, HitLocation, DamageType, DamageCauser->GetInstigator());
		 
		if (OwnerHealth->GetHealth() <= 0)
		{
			// TODO: Check if player character or enemy
			// TODO: If enemy delete, if player, do extra work on player and send to GameMode for any state change
			OnDeath(DamageCauser->GetInstigator());
		}
	}
}

float UHealthComponent::ProcessDamage(float Damage)
{
	// For child class to do any extra logic with damage application
	return Damage;
}

void UHealthComponent::OnDeath(AActor* ActorThatKilled)
{
	OnDeathDelegate.Broadcast(GetOwner());
	OwnerHealth->OnDeath(ActorThatKilled);
}

void UHealthComponent::Stun(float StunDuration, AActor* DamageCause)
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

void UHealthComponent::KnockBack(FVector KnockBackForce, AActor* DamageCause)
{
	if (OwnerHealth == nullptr) return;
	
	ABlindEyeBaseCharacter* Character = Cast<ABlindEyeBaseCharacter>(GetOwner());
	if (Character)
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->AddImpulse(KnockBackForce * OwnerHealth->GetMass());
		KnockBackDelegate.Broadcast(KnockBackForce);
	}
}

bool UHealthComponent::GetIsHunterDebuff()
{
	if (CurrMark.bHasMark)
	{
		return CurrMark.MarkerType == EMarkerType::Hunter;
	}
	return false;
}

void UHealthComponent::Burn(float DamagePerSec, float Duration, AActor* DamageCause)
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

void UHealthComponent::Stagger(AActor* DamageCause)
{
	// TODO: probably call stun?
	// ...
}

void UHealthComponent::TryApplyMarker(EMarkerType MarkerType, AActor* DamageCause)
{
	UWorld* world = GetWorld();
	if (!world) return;

	// prevent adding marks to from a player to another player
	if (MarkerType != EMarkerType::Hunter)
	{
		if (const ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner()))
		{
			return;
		}
	}
	// Otherwise Hunter mark being applied
	else
	{
		// If Current marker exists and is not hunter, replcae it
		if (CurrMark.bHasMark && CurrMark.MarkerType != EMarkerType::Hunter)
		{
			MarkedRemovedDelegate.Broadcast();
			AddMarkerHelper(MarkerType);
		}
		else
		{
			AddMarkerHelper(MarkerType);
		}
	}

	// If marked by hunter, dont do anything else
	if (CurrMark.bHasMark && CurrMark.MarkerType == EMarkerType::Hunter)
	{
		return;
	}
	
	if (CurrMark.bHasMark)
	{
		// Refresh the Mark if same mark used on enemy
		float TimeRemaining = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(world, MarkerDecayTimerHandle);
		float RefreshedTime = UKismetMathLibrary::Min(TimeRemaining + RefreshMarkerAmount, MarkerDecay);
		world->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
		world->GetTimerManager().SetTimer(MarkerDecayTimerHandle, this, &UHealthComponent::RemoveMark, RefreshedTime, false);
		RefreshMarkDelegate.Broadcast();
	} else
	{
		// Set the decay timer on marker
		world->GetTimerManager().SetTimer(MarkerDecayTimerHandle, this, &UHealthComponent::RemoveMark, MarkerDecay, false);
		AddMarkerHelper(MarkerType);
	}
}


void UHealthComponent::AddMarkerHelper(EMarkerType MarkerType)
{
	CurrMark.SetMark(MarkerType);
	MarkedAddedDelegate.Broadcast(MarkerType);
}



void UHealthComponent::TryDetonation(EPlayerType PlayerType, AActor* DamageCause)
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	if (CurrMark.bHasMark)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
		// Detonate mark if of different type, clear decay timer
		if (CurrMark.MarkerType == EMarkerType::Crow && PlayerType != EPlayerType::CrowPlayer ||
			CurrMark.MarkerType == EMarkerType::Phoenix && PlayerType != EPlayerType::PhoenixPlayer ||
			(Player != nullptr && CurrMark.MarkerType == EMarkerType::Hunter && Player->PlayerType != PlayerType))
		{
			world->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
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
		// Stun to detonated enemy (Detonate Crow Mark on Enemy)
		if (CurrMark.MarkerType == EMarkerType::Crow)
		{
			UBaseDamageType* StunDamage = NewObject<UBaseDamageType>(GetTransientPackage(), DarkDetonationOnEnemyDamageType);
			SetDamage(DarkDetonationOnEnemyDamage, GetOwner()->GetActorLocation(), StunDamage, DamageCause);
		}
		// Burn on detonated (Detonate Phoenix Mark on Enemy)
		else if (CurrMark.MarkerType == EMarkerType::Phoenix)
		{
			UBaseDamageType* BurnDamage = NewObject<UBaseDamageType>(GetTransientPackage(), FireDetonationOnEnemyDamageType);
			SetDamage(1, GetOwner()->GetActorLocation(), BurnDamage, DamageCause);
		}
	}
	// If player being detonated
	else if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		// Explosion around detonated player (Detonate Crow Mark on Phoenix)
		if (CurrMark.MarkerType == EMarkerType::Crow)
		{
			UWorld* World = GetWorld();
			if (!World) return;
			
			UGameplayStatics::ApplyRadialDamage(World, DarkDetonationOnPlayerDamage, GetOwner()->GetActorLocation(),
				DarkDetonationOnPlayerRadius, DarkDetonationOnPlayerDamageType, TArray<AActor*>(), DamageCause);
			UBaseDamageType* ExplosionDamage = NewObject<UBaseDamageType>(GetTransientPackage(), DarkDetonationOnPlayerDamageType);
			SetDamage(1, GetOwner()->GetActorLocation(), ExplosionDamage, DamageCause);
		}
		// Healing well around detonated player (Detonate Phoenix Mark on Crow)
		else if (CurrMark.MarkerType == EMarkerType::Phoenix)
		{
			UWorld* World = GetWorld();
			if (!World) return;

			FActorSpawnParameters params;
			params.Instigator = Cast<APawn>(GetOwner());
			params.Owner = GetOwner();
			AHealingWell* HealingWell = World->SpawnActor<AHealingWell>(HealingWellType, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, params);
			HealingWell->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void UHealthComponent::TryTaunt(float Duration, AActor* Taunter)
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
	UWorld* World = GetWorld();
	if (World == nullptr)  return;

	World->GetTimerManager().ClearTimer(MarkerDecayTimerHandle);
	CurrMark.RemoveMark();
	MarkedRemovedDelegate.Broadcast();
}

FMarkData& UHealthComponent::GetCurrMark()
{
	return CurrMark;
}

void UHealthComponent::DetonateMark()
{
	DetonateDelegate.Broadcast();
	CurrMark.RemoveMark();
}


void UHealthComponent::Kill()
{
	OnDeath(nullptr);
}

void UHealthComponent::ImprovedHealing(float HealPercentIncrease, float Duration)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AppliedStatusEffects.IsImprovedHealing)
	{
		float RemainingTimeOnHeal = UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(World, ImprovedHealingTimerHandle);
		// Do nothing if trying to override with less heal duration
		if (Duration <= RemainingTimeOnHeal)
		{
			return;
		}
	}

	bool TempIsImprovedHealing = AppliedStatusEffects.IsImprovedHealing;
	
	World->GetTimerManager().SetTimer(ImprovedHealingTimerHandle, this, &UHealthComponent::RemoveImprovedHealing, Duration, false);
	AppliedStatusEffects.IsImprovedHealing = true; 
	AppliedStatusEffects.HealPercentIncrease = HealPercentIncrease;
	
	// only broadcast if start healing
	if (!TempIsImprovedHealing)
	{
		ImprovedHealingStartDelegate.Broadcast(HealPercentIncrease, Duration);
	}
}

void UHealthComponent::PerformHeal()
{
	if (HealPerSec <= 0) return;
	
	float HealAmountPerSec = HealPerSec;
	// Check if increased healing applied
	if (AppliedStatusEffects.IsImprovedHealing)
	{
		HealAmountPerSec += HealAmountPerSec * AppliedStatusEffects.HealPercentIncrease;
	}
	float NewHealth = OwnerHealth->GetHealth() + ((HealAmountPerSec / 100) * PerformHealDelay);
	NewHealth = FMath::Min(OwnerHealth->GetMaxHealth(), NewHealth);
	OwnerHealth->SetHealth(NewHealth);
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

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, IsInvincible);
	DOREPLIFETIME(UHealthComponent, CurrMark);
}