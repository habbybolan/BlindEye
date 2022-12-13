// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/ScreenIndicator.h"
#include "IndicatorManagerComponent.generated.h"

USTRUCT()
struct FIndicatorData
{ 
	GENERATED_BODY()

	void Initialize(const FName& indicatorID, UScreenIndicator* screenIndicator, float maxDuration)
	{
		IndicatorID = indicatorID;
		ScreenIndicator = screenIndicator;
		MaxDuration = maxDuration;
	}

	UPROPERTY()
	UScreenIndicator* ScreenIndicator;

	UPROPERTY()
	float MaxDuration;

	UPROPERTY()
	float CurrDuration = 0;

	FName IndicatorID;
};

class UScreenIndicator;
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UIndicatorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIndicatorManagerComponent();

	/**
	 * Add a new indicator to player's screen with an id and duration.
	 * @param IndicatorID	ID of indicator to add for removal later
	 * @param Duration		Duration for indicator to stay, 0 if indefinite duration
	 */
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CLI_AddIndicator(const FName IndicatorID, TSubclassOf<UScreenIndicator> ScreenIndicatorType, UObject* Target, float Duration);
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CLI_RemoveIndicator(const FName& IndicatorID);

	UFUNCTION(BlueprintCallable)
	UScreenIndicator* GetIndicator(const FName& IndicatorID);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TMap<FName, FIndicatorData> ShowingScreenIndicators;

	const float UpdateDurationDelay = 0.5;
	FTimerHandle UpdateDurationsTimerHandle;
	void UpdateDurations();

	UFUNCTION()
	void RemoveLostReferencedIndicator(FName IndicatorID);
};
