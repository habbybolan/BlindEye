// Copyright (C) Nicholas Johnson 2022

#include "Enemies/BlindEyeEnemybase.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABlindEyeEnemyBase::ABlindEyeEnemyBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Team = TEAMS::Enemy;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("Health Bar");
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize(FVector2D(200, 10));
	HealthBar->SetIsReplicated(true);
	HealthBar->SetRelativeRotation(FRotator(0, 90, 0));
	HealthBar->SetRelativeScale3D(FVector(.2f, .2f, .2f));
	HealthBar->SetRelativeLocation(FVector(0, 0, 100));
	HealthBar->SetupAttachment(GetMesh());
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
	CurrHealth = MaxHealth;

	// initialize enemy reference on game state for easy access
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)

		BlindEyeGS->SubscribeToEnemy(this);
	}

	// TODO: Start as false when fixed healthbar overlap issue
	HealthBar->SetVisibility(true);
}

void ABlindEyeEnemyBase::DestroyEnemy()
{
	Destroy();
}

void ABlindEyeEnemyBase::ApplyPulse(ABlindEyePlayerCharacter* PlayerEffectToApply)
{
	UGameplayStatics::ApplyPointDamage(this, 100000, FVector::ZeroVector, FHitResult(), PlayerEffectToApply->GetController(),
		PlayerEffectToApply, UBaseDamageType::StaticClass());
}

void ABlindEyeEnemyBase::SetHealthbarVisibility(bool IsVisible)
{
	// TODO: Make not permanent here
	HealthBar->SetVisibility(true);
}

void ABlindEyeEnemyBase::MULT_PlayAnimMontage_Implementation(UAnimMontage* AnimToPlay)
{
	PlayAnimMontage(AnimToPlay);
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
	if (bIsDead) return;
	
	Super::OnDeath(ActorThatKilled);
	bIsDead = true;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABlindEyeEnemyBase::DestroyEnemy, DestroyDelay, false);
	}
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
