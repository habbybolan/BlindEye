// Copyright (C) Nicholas Johnson 2022


#include "HUD/ScreenIndicator.h"

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

void UScreenIndicator::SetTarget(AActor* target)
{
	Target = target;
	FAnchors Anchor;
	Anchor.Minimum = FVector2D::ZeroVector;
	Anchor.Maximum = FVector2D::ZeroVector;
	SetAnchorsInViewport(Anchor);
}

void UScreenIndicator::FindScreenEdgeLocationForWorldLocation(FVector2D& OutScreenPosition,
	float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D ScreenPosition = FVector2D();
	
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter =  FVector2D(ViewportSize.X/2, ViewportSize.Y/2);
	
	
	APlayerController* PlayerController = GetOwningPlayer();

	FVector ViewportLocation;
	FRotator ViewportRotation;
	PlayerController->GetPlayerViewPoint(ViewportLocation, ViewportRotation);

	PlayerController->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPosition);
	OutScreenPosition = ScreenPosition;
	return; 
	
	FVector Forward = ViewportRotation.Vector();
	FVector Offset = (Target->GetActorLocation() - ViewportLocation);
	Offset.Normalize();
	
	float DotProduct = FVector::DotProduct(Forward, Offset);
	bool bLocationIsBehindCamera = (DotProduct < 0);
	
	if (bLocationIsBehindCamera)
	{
		// For behind the camera situation, we cheat a little to put the
		// marker at the bottom of the screen so that it moves smoothly
		// as you turn around. Could stand some refinement, but results
		// are decent enough for most purposes.
		
		FVector DiffVector = Target->GetActorLocation() - ViewportLocation;
		FVector Inverted = DiffVector * -1.f;
		FVector NewInLocation = ViewportLocation * Inverted;
		
		NewInLocation.Z -= 5000;
		
		PlayerController->ProjectWorldLocationToScreen(NewInLocation, ScreenPosition);
		ScreenPosition.Y = (EdgePercent * ViewportCenter.X) * 2.f;
		ScreenPosition.X = -ViewportCenter.X - ScreenPosition.X;
	}
	
	PlayerController->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPosition);
	
	// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
	if (ScreenPosition.X >= 0.f && ScreenPosition.X <= ViewportSize.X
		&& ScreenPosition.Y >= 0.f && ScreenPosition.Y <= ViewportSize.Y)
	{
		OutScreenPosition = ScreenPosition;
		bIsOnScreen = true;
		return;
	}
	
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
	
}
