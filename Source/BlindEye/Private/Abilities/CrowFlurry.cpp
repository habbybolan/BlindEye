// Copyright (C) Nicholas Johnson 2022


#include "Abilities/CrowFlurry.h"

#include "Kismet/GameplayStatics.h"

ACrowFlurry::ACrowFlurry()
{
	AbilityStates.Add(new UFirstCrowFlurryState(this));
}

void ACrowFlurry::StartCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

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
	// TODO: BoxTrace to damage any enemy inside
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.2f, FColor::Silver, "Crow Flurry Performing");
}

void ACrowFlurry::StopCrowFlurry()
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().ClearTimer(CrowFlurryTimerHandle);
	// TODO: Stop particles/sound...
}


// **** States *******

// First Crow Flurry state *********************

UFirstCrowFlurryState::UFirstCrowFlurryState(AAbilityBase* ability) : FAbilityState(ability) {}

bool UFirstCrowFlurryState::TryEnterState(EAbilityInputTypes abilityUsageType)
{
	if (CurrInnerState > EInnerState::None) return false;
	RunState();
	return true;
}

void UFirstCrowFlurryState::RunState(EAbilityInputTypes abilityUsageType)
{
	FAbilityState::RunState();

	if (!Ability) return;
	ACrowFlurry* CrowFlurry = Cast<ACrowFlurry>(Ability);
	if (!CrowFlurry) return;
	CrowFlurry->StartCrowFlurry();

	// leave running state on ability released
	if (abilityUsageType == EAbilityInputTypes::Released)
	{
		CrowFlurry->StopCrowFlurry();
		ExitState();
	}
}

void UFirstCrowFlurryState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}


