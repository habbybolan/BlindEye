// Copyright (C) Nicholas Johnson 2022


#include "Components/IndicatorManagerComponent.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "HUD/ScreenIndicator.h"

class UOverlay;

UIndicatorManagerComponent::UIndicatorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UIndicatorManagerComponent::CLI_AddIndicator_Implementation(const FName IndicatorID, TSubclassOf<UScreenIndicator> ScreenIndicatorType,
	UObject* Target, float Duration)
{
	if (ShowingScreenIndicators.Contains(IndicatorID)) return;

	//UOverlay* overlay = Cast<UOverlay>(_rootWidget->GetRootWidget());
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetOwner());
	check(Player)

	UOverlay* overlay = Cast<UOverlay>(Player->GameplayHud->GetRootWidget());
	
	if (IIndicatorInterface* IndicatorInterface = Cast<IIndicatorInterface>(Target))
	{
		if (UWorld* World = GetWorld())
		{
			if (Player->GetController())
			{
				APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
				UWidget* Widget = CreateWidget(PlayerController, ScreenIndicatorType, FName(IndicatorID));
				UScreenIndicator* ScreenIndicator = Cast<UScreenIndicator>(Widget);
				ScreenIndicator->SetTarget(Target);
				ScreenIndicator->IndicatorID = IndicatorID;

				UOverlaySlot* slot = overlay->AddChildToOverlay(ScreenIndicator);

				// Set the child widget to fill overlay area to allow internal anchoring to work
				slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			
				//ScreenIndicator->AddToViewport();
				FIndicatorData ScreenIndicatorData;
				ScreenIndicatorData.Initialize(IndicatorID, ScreenIndicator, Duration);
				ShowingScreenIndicators.Add(IndicatorID, ScreenIndicatorData);
			}
			
		}
	}
}

void UIndicatorManagerComponent::CLI_RemoveIndicator_Implementation(const FName& IndicatorID)
{ 
	if (ShowingScreenIndicators.Contains(IndicatorID))
	{
		UScreenIndicator* IndicatorToDelete = ShowingScreenIndicators[IndicatorID].ScreenIndicator;
		IndicatorToDelete->RemoveFromParent();
		// let garbage collection remove it
		ShowingScreenIndicators[IndicatorID].ScreenIndicator = nullptr;
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
	TArray<FName> IndicatorIDsToDelete;
	// Update durations on all screen indicators with a definite duration
	for (TPair<FName, FIndicatorData>& ScreenIndicator : ShowingScreenIndicators)
	{
		if (ScreenIndicator.Value.MaxDuration > 0)
		{
			ScreenIndicator.Value.CurrDuration += UpdateDurationDelay;
			// If passed max duration, then remove screen indicator
			if (ScreenIndicator.Value.CurrDuration >= ScreenIndicator.Value.MaxDuration)
			{
				IndicatorIDsToDelete.Add(ScreenIndicator.Value.IndicatorID);
			}
		}
		// If lost reference to target, remove indicator
		else if (ScreenIndicator.Value.ScreenIndicator->GetTarget() == nullptr)
		{
			IndicatorIDsToDelete.Add(ScreenIndicator.Value.IndicatorID);
		}
	}

	// Delete all indicators signalled for deletion
	for (FName IndicatorIDToDelete : IndicatorIDsToDelete)
	{
		CLI_RemoveIndicator(IndicatorIDToDelete);
	}
}

void UIndicatorManagerComponent::RemoveLostReferencedIndicator(FName IndicatorID)
{
	CLI_RemoveIndicator(IndicatorID);
}

