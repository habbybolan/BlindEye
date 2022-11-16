// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "EnemyTutorialTextSnippet.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UEnemyTutorialTextSnippet : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	APlayerController* PlayerController;

	UFUNCTION(BlueprintCallable)
	void AddLineToDraw(UUserWidget* FromWidget, ABlindEyeEnemyBase* ToEnemy);
	
	virtual void NativeConstruct() override;
	
	TArray<TPair<UUserWidget*, ABlindEyeEnemyBase*>> LinesToDraw;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UCanvasPanel* TextSnippetContainer;
	
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual void RemoveFromParent() override;
};
