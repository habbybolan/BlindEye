// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingIcon.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ULoadingIcon : public UUserWidget
{
	GENERATED_BODY()

public:

	ULoadingIcon(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UWidget* LoadingWidget;

	UPROPERTY(EditDefaultsOnly)
	float OpacityChangeTime = 0.5;

	UFUNCTION(BlueprintCallable)
	void PlayLoadingIcon();

	UFUNCTION(BlueprintCallable)
	void StopLoadingIcon();


protected:
	
	FTimerHandle OpacityTimerHandle;
	UFUNCTION()
	void ShowIconHelper();
	UFUNCTION()
	void HideIconHelper();

	float OpacityChangeDelay = 0.02;
	
};
