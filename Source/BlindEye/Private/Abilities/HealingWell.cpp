// Copyright (C) Nicholas Johnson 2022


#include "Abilities/HealingWell.h"

#include "BlindEyeBaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AHealingWell::AHealingWell()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AHealingWell::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetActorRotation(CachedSpawnedRotation);
}

// Called when the game starts or when spawned
void AHealingWell::BeginPlay()
{
	Super::BeginPlay();

	CachedSpawnedRotation = GetActorRotation();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().SetTimer(HealingCheckTimerHandle, this, &AHealingWell::PerformHealCheck
		, HealCheckDelay, true);

	World->GetTimerManager().SetTimer(DelayedDestroyTimerHandle, this, &AHealingWell::DelayedDestroy
	, Duration, false);
}

void AHealingWell::PerformHealCheck()
{
	// TODO: Check overlapped actors with sphere overlap
	//		Apply improved healing to HealthComponent for HealCheckDelay duration

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	UKismetSystemLibrary::DrawDebugSphere(World, GetActorLocation(), Radius, 12, FColor::Green, HealCheckDelay);

	TArray<AActor*> OutActors;
	if (UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), Radius, ObjectTypes, nullptr, TArray<AActor*>(),OutActors))
	{
		for (AActor* Actor : OutActors)
		{
			if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Actor))
			{
				UHealthComponent* HealthComponent = HealthInterface->GetHealthComponent();
				if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
				{
					DamageInterface->ImprovedHealing(HealPercentIncr, HealCheckDelay * 2);
				}
			}
		}
	}
}

void AHealingWell::BeginDestroy()
{
	Super::BeginDestroy();
}

void AHealingWell::DelayedDestroy()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().ClearTimer(HealingCheckTimerHandle);
	
	BP_HealingWellDestroying();
	
	World->GetTimerManager().SetTimer(DelayedDestroyTimerHandle, this, &AHealingWell::DestroyHealingWell, DelayedDestructionTime, false);
}

void AHealingWell::DestroyHealingWell()
{
	Destroy();
}



