// Copyright (C) Nicholas Johnson 2022


#include "HUD/ScreenIndicator.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UScreenIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if (Target)
	// {
	// 	FVector2D OutScreenPosition;
	// 	float OutRotationAngleDegrees;
	// 	bool bIsOnScreen;
	// 	FindScreenEdgeLocationForWorldLocation(OutScreenPosition, OutRotationAngleDegrees, bIsOnScreen);
	//
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Green, OutScreenPosition.ToString());
	// 	SetPositionInViewport(OutScreenPosition);
	// }
}

void UScreenIndicator::SetTarget(UObject* target)
{
	Target = MakeWeakObjectPtr(target);
	FAnchors Anchor;
	Anchor.Minimum = FVector2D::ZeroVector;
	Anchor.Maximum = FVector2D::ZeroVector;
	SetAnchorsInViewport(Anchor);
}

UObject* UScreenIndicator::GetTarget()
{
	return Target.Get();
}

void UScreenIndicator::RemoveFromParent()
{
	Super::RemoveFromParent();

	if (IndicatorContainer)
	{
		for (UWidget* Child : IndicatorContainer->GetAllChildren())
		{
			Child->RemoveFromParent();
		}
	}
}

void UScreenIndicator::FindScreenEdgeLocationForWorldLocation(FVector2D& OutScreenPosition,
                                                              float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	if (!Target.IsValid()) return;
	IIndicatorInterface* TargetInterface = Cast<IIndicatorInterface>(Target.Get());
	if (!TargetInterface) return;
	
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D ScreenPosition = FVector2D();
	
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter =  FVector2D(ViewportSize.X/2, ViewportSize.Y/2);
	
	APlayerController* PlayerController = GetOwningPlayer();

	FVector ViewportLocation;
	FRotator ViewportRotation;
	PlayerController->GetPlayerViewPoint(ViewportLocation, ViewportRotation);
	
	FVector Forward = ViewportRotation.Vector();
	FVector Offset = (TargetInterface->GetIndicatorPosition() - ViewportLocation);
	Offset.Normalize();
	
	float DotProduct = FVector::DotProduct(Forward, Offset);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Purple, FString::SanitizeFloat(DotProduct));
	bool bLocationIsBehindCamera = (DotProduct < 0);
	
	if (bLocationIsBehindCamera)
	{
		FVector DirFromTarget = ViewportLocation - TargetInterface->GetIndicatorPosition();
		FVector NewPos = TargetInterface->GetIndicatorPosition() + DirFromTarget * 2;
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, NewPos, ScreenPosition, false);
		ScreenPosition.X += (ViewportCenter.X - ScreenPosition.X) * 2;
		ScreenPosition.Y = ViewportSize.Y * EdgePercent;

		if (ScreenPosition.X < (1 - EdgePercent) * ViewportSize.X) ScreenPosition.X = (1 - EdgePercent) * ViewportSize.X;
		if (ScreenPosition.X > ViewportSize.X) ScreenPosition.X = ViewportSize.X;

		OutScreenPosition = ScreenPosition;

		ScreenPosition -= ViewportCenter;
		float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
		AngleRadians -= FMath::DegreesToRadians(90.f);
		OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;
		
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Orange, FString::SanitizeFloat(OutRotationAngleDegrees));
		bIsOnScreen = false;
		return;
	}

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, TargetInterface->GetIndicatorPosition(), ScreenPosition, false);
	
	// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
	if (ScreenPosition.X >= 0.f && ScreenPosition.X <= ViewportSize.X
		&& ScreenPosition.Y >= 0.f && ScreenPosition.Y <= ViewportSize.Y)
	{
		OutScreenPosition = ScreenPosition;
		NormalizeScreenCoordinates(OutScreenPosition, ViewportSize);
		bIsOnScreen = true;
		return;
	}

	// Normalize to viewport center being origin
	ScreenPosition -= ViewportCenter;

	float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);
	
	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;
	
	float Cos = cosf(AngleRadians);
	float Sin = -sinf(AngleRadians);
	
	ScreenPosition = FVector2D(ViewportCenter.X + (Sin * 150.f), ViewportCenter.Y + Cos * 150.f);
	
	float m = Cos / Sin;
	
	FVector2D ScreenBounds = ViewportCenter * EdgePercent;
	
	if (Cos > 0)
	{
		ScreenPosition = FVector2D(ScreenBounds.Y/m, ScreenBounds.Y);
	}
	else
	{
		ScreenPosition = FVector2D(-ScreenBounds.Y/m, -ScreenBounds.Y);
	}
	
	if (ScreenPosition.X > ScreenBounds.X)
	{
		ScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X*m);
	}
	else if (ScreenPosition.X < -ScreenBounds.X)
	{
		ScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
	}
	
	ScreenPosition += ViewportCenter;
	OutScreenPosition = ScreenPosition;
	NormalizeScreenCoordinates(OutScreenPosition, ViewportSize);
}

void UScreenIndicator::NormalizeScreenCoordinates(FVector2D& OutScreenPosition, const FVector2D ViewportSize)
{
	OutScreenPosition.X = (1920 / ViewportSize.X * OutScreenPosition.X)/* / 1920*/;
	OutScreenPosition.Y = (1080 / ViewportSize.Y * OutScreenPosition.Y);/* / 1080*/;
}
