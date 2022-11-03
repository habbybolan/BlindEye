// Copyright (C) Nicholas Johnson 2022


#include "HUD/W_Radar.h"

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
		for (AActor* Actor : BlindEyeGS->GetAllEnemies())
		{
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
	FVector ViewportLocation;
	FRotator ViewportRotation;

	PlayerController->GetPlayerViewPoint(ViewportLocation, ViewportRotation);
	FVector VecToEnemy = Actor->GetActorLocation() - ViewportLocation;
	FRotator RotationToEnemy = UKismetMathLibrary::FindLookAtRotation(ViewportRotation.Vector(), VecToEnemy);
	float YawRotation = RotationToEnemy.Yaw;
	YawRotation -= (360 / NumPiecesInASection) / 2;

	uint8 Index;
	float Remainder;
	
	if (YawRotation > 0)
	{
		Index = UKismetMathLibrary::FMod(YawRotation, 360 / NumPiecesInASection, Remainder);
	} else
	{
		YawRotation += 180;
		Index = UKismetMathLibrary::FMod(YawRotation, 360 / NumPiecesInASection, Remainder);
	}

	// If enemy in first section
	if (VecToEnemy.Size() <= MaxDistForFirstSection)
	{
		return Index;
	}
	// If enemy in second section
	else if (MaxDistForSecondSection == 0 || VecToEnemy.Size() <= MaxDistForFirstSection)
	{
		return Index + NumSections;
	}
	return -1;
}
