// Copyright (C) Nicholas Johnson 2022


#include "HUD/EnemyTutorialTextSnippet.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UEnemyTutorialTextSnippet::AddLineToDraw(UUserWidget* FromWidget, ABlindEyeEnemyBase* ToEnemy)
{
	LinesToDraw.Add(TPairInitializer<UUserWidget*, ABlindEyeEnemyBase*>(FromWidget, ToEnemy));
}

void UEnemyTutorialTextSnippet::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerController = GetOwningPlayer();
}

int32 UEnemyTutorialTextSnippet::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                             const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                             const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	//
	// if (PlayerController)
	// {
	// 	UWorld* World = GetWorld();
	// 	if (World)
	// 	{
	// 		for (TPair<UUserWidget*, ABlindEyeEnemyBase*> LineToDraw : LinesToDraw)
	// 		{
	// 			if (!LineToDraw.Key) continue; 
	// 			FVector2D StartPos = UWidgetLayoutLibrary::SlotAsCanvasSlot(LineToDraw.Key)->GetPosition();
	// 			FVector2D EndPos;
	// 			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, LineToDraw.Value->GetActorLocation(), EndPos, false);
	// 			UWidgetBlueprintLibrary::DrawLine(Context, StartPos ,EndPos);
	// 		}
	// 	}
	// }
	
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UEnemyTutorialTextSnippet::RemoveFromParent()
{
	Super::RemoveFromParent();

	if (TextSnippetContainer)
	{
		for (UWidget* Child : TextSnippetContainer->GetAllChildren())
		{
			Child->RemoveFromParent();
		}
	}
	
}
