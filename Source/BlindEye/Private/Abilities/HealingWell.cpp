// Copyright (C) Nicholas Johnson 2022


#include "Abilities/HealingWell.h"

#include "BlindEyeBaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AHealingWell::AHealingWell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InitialLifeSpan = Duration;
}

// Called when the game starts or when spawned
void AHealingWell::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	World->GetTimerManager().SetTimer(HealingCheckTimerHandle, this, &AHealingWell::PerformHealCheck
		, HealCheckDelay, true);
	SpawnParticles();
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
				if (const IDamageInterface* DamageInterface = Cast<IDamageInterface>(HealthComponent))
				{
					DamageInterface->Execute_ImprovedHealing(HealthComponent, HealPercentIncr, HealCheckDelay * 2);
				}
			}
		}
	}
}

void AHealingWell::SpawnParticles()
{
	// TODO:
}

void AHealingWell::KillParticles()
{
	// TODO:
}

void AHealingWell::BeginDestroy()
{
	Super::BeginDestroy();
	KillParticles();
}



