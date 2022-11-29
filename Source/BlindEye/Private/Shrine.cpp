// Copyright (C) Nicholas Johnson 2022
#pragma optimize("", off)

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

	IndicatorPosition = CreateDefaultSubobject<USceneComponent>("IndicatorPosition");
	IndicatorPosition->SetupAttachment(Mesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
}

void AShrine::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// TODO: REmove, used for testing points
	if (GetLocalRole() == ROLE_Authority)
	{
		for (UShrineAttackPoint* Point : AttackPoints)
		{
			UKismetSystemLibrary::DrawDebugSphere(World, Point->Location, 25);
		}
	}
}


void AShrine::BeginPlay()
{
	Super::BeginPlay();

	CurrShrineHealth = MaxShrineHealth; 

	if (GetLocalRole() == ROLE_Authority)
	{
		UWorld* World = GetWorld();
		check(World)
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		BlindEyeGS->GameStartedDelegate.AddDynamic(this, &AShrine::MULT_OnGameStarted);

		InitializeAttackPoint();
	}
}

void AShrine::InitializeAttackPoint()
{
	FVector FrontSpot = GetActorLocation() + FVector::ForwardVector * CapsuleComponent->GetScaledCapsuleRadius() + FVector::ForwardVector * AttackPointDistOffset;
	float DegreesBtwPoints = 360 / NumSurroundingAttackPoints;
	for (uint8 i = 0; i < NumSurroundingAttackPoints; i++)
	{
		UShrineAttackPoint* ShrineAttackPoint = NewObject<UShrineAttackPoint>(GetTransientPackage());

		FVector AttackLocation = FrontSpot.RotateAngleAxis(DegreesBtwPoints * i, FVector::UpVector);
		ShrineAttackPoint->Initialize(AttackLocation);
		AttackPoints.Add(ShrineAttackPoint);
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
	return IndicatorPosition->GetComponentLocation();
}

UShrineAttackPoint* AShrine::AskForClosestPoint(ASnapperEnemy* Snapper)
{
	// No open attack points
	if (!IsOpenAttackPoint()) return false;
		
	FVector AskerLocation = Snapper->GetActorLocation();
	uint8 ClosestAttackPoint = FindClosestAttackPointIndex(AskerLocation);

	// TODO: Shifting logic

	PerformShift(ClosestAttackPoint, Snapper);
	return AttackPoints[ClosestAttackPoint];
}

uint8 AShrine::FindClosestAttackPointIndex(const FVector& AskerLocation)
{
	float ClosestDist = FVector::Dist(AttackPoints[0]->Location, AskerLocation);
	uint8 ClosestIndex = 0;
	uint8 Index = 0;
	for (Index = 1; Index < AttackPoints.Num(); ++Index)
	{
		UShrineAttackPoint* AttackPoint = AttackPoints[Index];
		float Dist = FVector::Dist(AttackPoint->Location, AskerLocation);
		if (Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestIndex = Index;
		}
	}

	return ClosestIndex;
}

bool AShrine::IsOpenAttackPoint()
{
	for (uint8 i = 0; i < AttackPoints.Num(); i++)
	{
		if (AttackPoints[i]->State == EShrineAttackPointState::Empty) return true;
	}
	return false;
}

void AShrine::PerformShift(int8 AttackPointIndex, ASnapperEnemy* AskingSnapper)
{
	// Check if first point is already empty
	if (!AttackPoints[AttackPointIndex]->State == EShrineAttackPointState::Empty)
	{
		bool IsRightClosest;
		uint8 ClosestOpenPointIndex = GetClosestOpenPointLeft(AttackPointIndex, OUT IsRightClosest);
		
		int8 CurrIndex = AttackPointIndex;
		TWeakObjectPtr<ASnapperEnemy> PrevSnapper = AttackPoints[AttackPointIndex]->SubscribedSnapper;
		// Perform shift on all points up to closest open point
		while (CurrIndex != ClosestOpenPointIndex)
		{
			AttackPoints[CurrIndex]->UnsubscribeSnapper();
			// Incr/Decr. Curr Index
			CurrIndex = IsRightClosest ? CurrIndex + 1 : CurrIndex - 1;
			if (CurrIndex < 0) CurrIndex = AttackPoints.Num() - 1;
			if (CurrIndex >= AttackPoints.Num()) CurrIndex = 0;
			
			UShrineAttackPoint* CurrPoint = AttackPoints[CurrIndex];
			TWeakObjectPtr<ASnapperEnemy> TempSnapper = AttackPoints[CurrIndex]->SubscribedSnapper;
			CurrPoint->PerformShift(PrevSnapper.Get());
			PrevSnapper = TempSnapper;
		}
	}

	// Subscribe snapper to its asking points
	AttackPoints[AttackPointIndex]->SubscribeSnapper(AskingSnapper);
}

uint8 AShrine::GetClosestOpenPointLeft(uint8 AttackPointIndex, bool& IsRightClosest)
{
	uint8 LeftIndex = AttackPointIndex;
	uint8 RightIndex = AttackPointIndex;

	// Find if right or left index has the closest point
	do 
	{
		LeftIndex = LeftIndex - 1 < 0 ? AttackPoints.Num() - 1 : LeftIndex - 1;
		RightIndex = RightIndex + 1 >= AttackPoints.Num() ? 0 : RightIndex + 1;
		// Check if right index is empty slot
		if (AttackPoints[RightIndex]->State == EShrineAttackPointState::Empty)
		{
			IsRightClosest = true;
			return RightIndex;
		}
		// Check if left index is empty slot
		else if (AttackPoints[LeftIndex]->State == EShrineAttackPointState::Empty)
		{
			IsRightClosest = false;
			return LeftIndex;
		}
	} while (LeftIndex != RightIndex);

	// Right and left index met
	return LeftIndex;
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

