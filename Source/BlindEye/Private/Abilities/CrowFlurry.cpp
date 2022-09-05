// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowFlurry.h"

#include "Interfaces/HealthInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ACrowFlurry::ACrowFlurry()
{
	AbilityStates.Add(new UFirstCrowFlurryState(this));
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	ParticleActor = world->SpawnActor<AActor>(AActor::StaticClass(), GetInstigator()->GetActorLocation(), GetInstigator()->GetControlRotation());
	
	SpawnedCrowFlurryParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(CrowFlurryParticle, GetInstigator()->GetRootComponent(), NAME_None,
		GetInstigator()->GetActorLocation(), GetInstigator()->GetControlRotation(), FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);

	world->GetTimerManager().SetTimer(CrowFlurryTimerHandle, this, &ACrowFlurry::PerformCrowFlurry, 0.2f, true);
}

void ACrowFlurry::PerformCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;
 
	FVector InstigatorFwd =  GetInstigator()->GetControlRotation().Vector() * Range;
	FVector TargetLocation = GetInstigator()->GetActorLocation() + InstigatorFwd;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), TargetLocation, FVector(0, Width / 2, Height / 2),
		GetInstigator()->GetControlRotation(), ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true,
		FLinearColor::Red, FLinearColor::Green, 0.2f);

	const IHealthInterface* InstigatorHealthInterface = Cast<IHealthInterface>(GetInstigator());
	TEAMS InstigatorTeam = InstigatorHealthInterface->Execute_GetTeam(GetInstigator());
	
	for (FHitResult Hit : OutHits)
	{
		if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(Hit.GetActor()))
		{
			if (HealthInterface->Execute_GetTeam(Hit.GetActor()) != InstigatorTeam)
			{
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamagePerSec * 0.2f, FVector::ZeroVector,
					FHitResult(), GetInstigator()->Controller, this, DamageType);
			}
		}
	}
}

void ACrowFlurry::StopCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	if (SpawnedCrowFlurryParticle)
	{
		SpawnedCrowFlurryParticle->Deactivate();
	}
	
	world->GetTimerManager().SetTimer(CrowFlurryParticleDestroyTimerHandle, this, &ACrowFlurry::DestroyParticles, 1.f, false);
	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
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
	if (CurrInnerState > EInnerState::None) return;
	RunState();
}

void UFirstCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();
	
	if (!Ability) return;
	Ability->Blockers.Add(EBlockers::Movement);
	Ability->Blockers.Add(EBlockers::OtherAbilities);
	
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


