// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Radar.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UW_Radar : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float MaxDistForFirstSection = 1000;

	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="0 if unlimited range"))
	float MaxDistForSecondSection = 0;

	UPROPERTY(EditDefaultsOnly)
	uint8 NumSections = 2;
 
	UPROPERTY(EditDefaultsOnly)
	uint8 NumPiecesInASection = 6;
	
	void UpdateRadar();

	int8 GetRadarIndex(AActor* Actor, APlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateRadar(const TArray<uint8>& RadarIndices);
};
