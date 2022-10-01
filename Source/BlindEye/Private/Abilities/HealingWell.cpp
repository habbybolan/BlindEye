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
	bReplicates = true;
	InitialLifeSpan = 2;
}

// Called when the game starts or when spawned
void AHealingWell::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	PerformHealCheck();
}

void AHealingWell::PerformHealCheck()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
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
					DamageInterface->ImprovedHealing(HealPercentIncr, HealDuration);
				}
			}
		}
	}
}



