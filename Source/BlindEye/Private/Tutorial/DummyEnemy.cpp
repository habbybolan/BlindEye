// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DummyEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tutorial/DummyEnemyHealthComponent.h"

ADummyEnemy::ADummyEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDummyEnemyHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetVisibility(false);
	
	DummyMesh = CreateDefaultSubobject<UStaticMeshComponent>("DummyMesh");
	DummyMesh->SetupAttachment(RootComponent);
	DummyMesh->SetVisibility(false);
}

void ADummyEnemy::KillDummy()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DeathTimerHandle, this, &ADummyEnemy::CustomDeath, DeathDelay, false);
		MULT_KilLDummyHelper();
	}
}

void ADummyEnemy::MULT_KilLDummyHelper_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		EndPos = GetActorLocation() - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 * FVector::UpVector;
		StartPos = GetActorLocation();
		World->GetTimerManager().SetTimer(AnimTimerHandle, this, &ADummyEnemy::SpawnDeathMovement, AnimDelay, true);
	}
}
void ADummyEnemy::BeginPlay()
{
	Super::BeginPlay();

	EndPos = GetActorLocation();
	StartPos = GetActorLocation() - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 * FVector::UpVector;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AnimTimerHandle, this, &ADummyEnemy::SpawnDeathMovement, AnimDelay, true);
	}
}

void ADummyEnemy::CustomDeath()
{
	Destroy();
}

void ADummyEnemy::SpawnDeathMovement()
{
	SetActorLocation(UKismetMathLibrary::VEase(StartPos, EndPos, CurrAnimDuration, bSPawning ? SpawningEaseFunc : DespawnngEaseFunc));
	DummyMesh->SetVisibility(true);
	CurrAnimDuration += AnimDelay;

	// Stop animation if reached end
	if (CurrAnimDuration >= AnimDuration)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(AnimTimerHandle);
			bSPawning = false;
		}
		CurrAnimDuration = 0;
	}
}
