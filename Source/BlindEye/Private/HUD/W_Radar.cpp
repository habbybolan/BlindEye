// Copyright (C) Nicholas Johnson 2022


#include "HUD/W_Radar.h"

#include "Enemies/Snapper/SnapperEnemy.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UW_Radar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateRadar();
}

void UW_Radar::UpdateRadar()
{
	APlayerController* PlayerController = GetOwningPlayer();
	TArray<uint8> NumOfEnemiesAtRadarIndices;
	NumOfEnemiesAtRadarIndices.SetNum(12);
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		for (ABlindEyeEnemyBase* Actor : BlindEyeGS->GetAllEnemies())
		{
			// dont show snappers on radar
			if (ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(Actor)) continue;
			
			int8 RadarIndex = GetRadarIndex(Actor, PlayerController);
			if (RadarIndex >= 0)
			{
				NumOfEnemiesAtRadarIndices[RadarIndex]++;
			}
		}
	}
	BP_UpdateRadar(NumOfEnemiesAtRadarIndices);
}

int8 UW_Radar::GetRadarIndex(AActor* Actor, APlayerController* PlayerController)
{
	if (Actor == nullptr) return -1;
	FVector ViewportLocation;
	FRotator ViewportRotation;

	PlayerController->GetPlayerViewPoint(ViewportLocation, ViewportRotation);
	FVector VecToEnemy = Actor->GetActorLocation() - ViewportLocation;
	float DistToEnemy = VecToEnemy.Size();
	
	FVector2D VecToEnemy2D = FVector2D(VecToEnemy.X, VecToEnemy.Y);
	VecToEnemy2D.Normalize();
	FVector2D ViewportLook2D = FVector2D(ViewportRotation.Vector().X, ViewportRotation.Vector().Y);
	ViewportLook2D.Normalize();
	
	float YawRotation = UKismetMathLibrary::Acos(
		UKismetMathLibrary::DotProduct2D(VecToEnemy2D, ViewportLook2D) /
		(VecToEnemy2D.Size() * ViewportLook2D.Size())) * (180 / UKismetMathLibrary::GetPI());

	FVector RightViewport = ViewportRotation.Vector().RotateAngleAxis(90, FVector(0, 0, 1));

	float Dot = UKismetMathLibrary::Dot_VectorVector(RightViewport, VecToEnemy);
	if (Dot < 0)
	{
		YawRotation *= -1;
	}

	float DegreesPerSection = 360 / NumPiecesInASection;
	//YawRotation += DegreesPerSection / 2;
	
	
	uint8 Index;

	// Top Section
	if (UKismetMathLibrary::Abs(YawRotation) < DegreesPerSection / 2)
	{
		Index = 0;
	}
	// Bottom section
	else if (UKismetMathLibrary::Abs(YawRotation) > 180 -  DegreesPerSection / 2)
	{
		Index = 3;
	}
	else if (UKismetMathLibrary::Abs(YawRotation) > DegreesPerSection / 2 &&
		UKismetMathLibrary::Abs(YawRotation) < (DegreesPerSection / 2 + DegreesPerSection))
	{
		if (YawRotation > 0) Index = 1;
		else Index = 5;
	}
	else
	{
		if (YawRotation > 0) Index = 2;
		else Index = 4;
	}
	
	// if (YawRotation > 0)
	// {
	// 	if (YawRotation < DegreesPerSection)
	// 	{
	// 		Index = 0;
	// 	} else
	// 	{
	// 		Index = UKismetMathLibrary::FMod(YawRotation, 360 / NumPiecesInASection, Remainder);
	// 	}
	// } else
	// {
	// 	YawRotation -= 180;
	// 	Index = UKismetMathLibrary::FMod(YawRotation, 360 / NumPiecesInASection, Remainder);
	// }

	//If enemy in first section
	if (DistToEnemy <= MaxDistForFirstSection)
	{
		return Index;
	}
	// If enemy in second section
	else if (MaxDistForSecondSection == 0 || DistToEnemy <= MaxDistForFirstSection)
	{
		return Index + NumPiecesInASection;
	}
	return -1;
}
