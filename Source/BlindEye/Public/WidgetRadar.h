// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "WidgetRadar.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UWidgetRadar : public UUserWidget
{
	GENERATED_BODY()

public:

	UWidgetRadar(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
 
	void UpdateRadar(TArray<AActor*> Enemies);
	
};
