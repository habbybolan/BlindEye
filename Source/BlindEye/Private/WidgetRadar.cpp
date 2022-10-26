// Copyright (C) Nicholas Johnson 2022


#include "WidgetRadar.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UWidgetRadar::UWidgetRadar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UWidgetRadar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World, ABlindEyeEnemyBase::StaticClass(), OutActors);
	UpdateRadar(OutActors);
}

void UWidgetRadar::UpdateRadar(TArray<AActor*> Enemies)
{
	uint8 Top = 0;
	uint8 TopRight = 0;
	uint8 TopLeft = 0;
	uint8 Bottom = 0;
	uint8 BottomRight = 0;
	uint8 BottomLeft = 0;

	APlayerController* Controller = GetOwningPlayer();
	
	FVector ViewportLocation;
	FRotator ViewportRotation;
	Controller->GetPlayerViewPoint(ViewportLocation, ViewportRotation);
	FVector FwdVec = ViewportRotation.Vector() * FVector(1, 1, 0);

	FVector FwdVecToEnemy;
	for (AActor* Enemy : Enemies)
	{
		FwdVecToEnemy = (Enemy->GetActorLocation() - ViewportLocation) * FVector(1, 1, 0);
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FwdVec, FwdVecToEnemy);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.02f, FColor::Orange, Rotation.ToString());
	}
}
