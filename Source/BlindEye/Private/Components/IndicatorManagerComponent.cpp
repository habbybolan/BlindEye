// Copyright (C) Nicholas Johnson 2022


#include "Components/IndicatorManagerComponent.h"

#include "HUD/ScreenIndicator.h"

UIndicatorManagerComponent::UIndicatorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UIndicatorManagerComponent::CLI_AddIndicator_Implementation(const FName IndicatorID, TSubclassOf<UScreenIndicator> ScreenIndicatorType,
	UObject* Target, float Duration)
{
	if (ShowingScreenIndicators.Contains(IndicatorID)) return;

	if (IIndicatorInterface* IndicatorInterface = Cast<IIndicatorInterface>(Target))
	{
		if (UWorld* World = GetWorld())
		{
			UScreenIndicator* ScreenIndicator = Cast<UScreenIndicator>(CreateWidget(World, ScreenIndicatorType, FName(IndicatorID)));
			ScreenIndicator->SetTarget(Target);
			ScreenIndicator->AddToViewport();
			FIndicatorData ScreenIndicatorData;
			ScreenIndicatorData.Initialize(IndicatorID, ScreenIndicator, Duration);
			ShowingScreenIndicators.Add(IndicatorID, ScreenIndicatorData);
		}
	}
}

void UIndicatorManagerComponent::CLI_RemoveIndicator_Implementation(const FName& IndicatorID)
{
	if (ShowingScreenIndicators.Contains(IndicatorID))
	{
		UScreenIndicator* IndicatorToDelete = ShowingScreenIndicators[IndicatorID].ScreenIndicator;
		IndicatorToDelete->RemoveFromParent();
		ShowingScreenIndicators.Remove(IndicatorID);
	}
}

UScreenIndicator* UIndicatorManagerComponent::GetIndicator(const FName& IndicatorID)
{
	if (ShowingScreenIndicators.Contains(IndicatorID))
	{
		return ShowingScreenIndicators[IndicatorID].ScreenIndicator;
	}
	return nullptr;
} 

void UIndicatorManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Add Timer to decrement indicator durations
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(UpdateDurationsTimerHandle, this, &UIndicatorManagerComponent::UpdateDurations, UpdateDurationDelay, true);
	}
}

void UIndicatorManagerComponent::UpdateDurations()
{
	// Update durations on all screen indicators with a definite duration
	for (TPair<FName, FIndicatorData>& ScreenIndicator : ShowingScreenIndicators)
	{
		if (ScreenIndicator.Value.MaxDuration > 0)
		{
			ScreenIndicator.Value.CurrDuration += UpdateDurationDelay;
			// If passed max duration, then remove screen indicator
			if (ScreenIndicator.Value.CurrDuration >= ScreenIndicator.Value.MaxDuration)
			{
				CLI_RemoveIndicator(ScreenIndicator.Value.IndicatorID);
			}
		}
	}
}

