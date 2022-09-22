// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowFlurry.h"

#include "Interfaces/HealthInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

ACrowFlurry::ACrowFlurry()
{
	AbilityStates.Add(new UFirstCrowFlurryState(this));
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().SetTimer(CrowFlurryTimerHandle, this, &ACrowFlurry::PerformCrowFlurry, 0.2f, true);

	APlayerController* PlayerController = Cast<APlayerController>(GetInstigator()->GetController());
	FVector Position;
	FVector Rotation;
	if (PlayerController->DeprojectMousePositionToWorld(Position, Rotation))
	{
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingle(world, Position, Position + Rotation * 1000, MouseRayTrace,
			false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, Hit, true))
		{
			FVector HitLocation = Hit.Location + FVector::UpVector * 30;
			FRotator rotation = (HitLocation - GetOwner()->GetActorLocation()).Rotation();
			MULT_SpawnCrowFlurry(rotation);
		}
	}
}

void ACrowFlurry::MULT_SpawnCrowFlurry_Implementation(FRotator rotation)
{
	MULT_SpawnCrowFlurryHelper();
}

void ACrowFlurry::PerformCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FVector InstigatorFwd;
	APlayerController* PlayerController = Cast<APlayerController>(GetInstigator()->GetController());
	FVector Position;
	FVector Rotation;
	if (PlayerController->DeprojectMousePositionToWorld(Position, Rotation))
	{
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingle(world, Position, Position + Rotation * 10000, MouseRayTrace,
			false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, Hit, true))
		{
			FVector HitLocation = Hit.Location + FVector::UpVector * 100;
			InstigatorFwd = HitLocation - GetOwner()->GetActorLocation();
		}
	}
 
	FVector TargetLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), TargetLocation, FVector(0, Width / 2, Height / 2),
		GetInstigator()->GetControlRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true,
		FLinearColor::Red, FLinearColor::Green, 0.2f);
	
	for (FHitResult Hit : OutHits)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamagePerSec * 0.2f, FVector::ZeroVector,
					FHitResult(), GetInstigator()->Controller, this, DamageType);
	}

	// consume bird meter
	if (!TryConsumeBirdMeter(CostPercentPerSec * 0.2f))
	{
		// if out of bird meter, simulate releasing button
		AbilityStates[CurrState]->HandleInput(EAbilityInputTypes::Released);
	}
}

void ACrowFlurry::MULT_DestroyCrowFlurry_Implementation()
{
	MULT_DestroyCrowFlurryHelper();
}

void ACrowFlurry::StopCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	MULT_DestroyCrowFlurry();
	
	// TODO: Stop particles/sound...
}

void ACrowFlurry::DestroyParticles()
{
	if (SpawnedCrowFlurryParticle)
	{
		SpawnedCrowFlurryParticle->DestroyComponent();
	}
}

void ACrowFlurry::EndAbilityLogic()
{
	Super::EndAbilityLogic();

	UWorld* world = GetWorld();
	if (!world) return;
	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
	world->GetTimerManager().ClearTimer(CrowFlurryParticleDestroyTimerHandle);
}

// **** States *******

// First Crow Flurry state *********************

UFirstCrowFlurryState::UFirstCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

void UFirstCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::TryEnterState();
	// apply initial cost
	if (!Ability) return;
	if (ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability))
	{
		if (!CrowFlurry->TryConsumeBirdMeter(CrowFlurry->InitialCostPercent)) return;
	}
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void UFirstCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	
	if (!Ability) return;
	Ability->Blockers.IsMovementBlocked = true;
	Ability->Blockers.IsOtherAbilitiesBlocked = true;
	
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (!CrowFlurry) return;
	

	// leave running state on ability released
	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		CrowFlurry->StopCrowFlurry();
		ExitState();
	} else
	{
		CrowFlurry->StartCrowFlurry();
	}
}

void UFirstCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

void ACrowFlurry::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


