// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HUD/ScreenIndicator.h"
#include "PlayerScreenIndicator.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UPlayerScreenIndicator : public UScreenIndicator
{
	GENERATED_BODY()

public:

	bool bIsPlayerMarked = false;
	bool bisPlayerDowned = false;

	virtual void SetTarget(UObject* target) override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

	bool bInitialized = false;
	
	void HideAllWidgets();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerMarkedOnScreenWidgetType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerMarkedOffScreenWidgetType; 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerDownedOnScreenWidgetType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerDownedOffScreenWidgetType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerNormalOffScreenWidgetType;

	UPROPERTY()
	UUserWidget* PlayerMarkedOnScreenWidget;
	UPROPERTY()
	UUserWidget* PlayerMarkedOffScreenWidget;
	UPROPERTY()
	UUserWidget* PlayerDownedOnScreenWidget;
	UPROPERTY()
	UUserWidget* PlayerDownedOffScreenWidget;
	UPROPERTY()
	UUserWidget* PlayerNormalOffScreenWidget;   
};
