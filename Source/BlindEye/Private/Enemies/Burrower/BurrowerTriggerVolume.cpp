// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerTriggerVolume.h"

UBurrowerTriggerVolume::UBurrowerTriggerVolume()
{
	SetGenerateOverlapEvents(true);
}

void UBurrowerTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UBurrowerTriggerVolume::OnPlayerOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBurrowerTriggerVolume::OnPlayerEndOverlap);
}

TArray<ABlindEyePlayerCharacter*> UBurrowerTriggerVolume::GetPlayerActorsOverlapping()
{
	return PlayersInsideTriggerVolume;
}

void UBurrowerTriggerVolume::OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		PlayersInsideTriggerVolume.AddUnique(Player);
		CustomOverlapStartDelegate.Broadcast(OverlappedComponent, OtherActor);
	}
}

void UBurrowerTriggerVolume::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	for (uint8 i = 0; i < PlayersInsideTriggerVolume.Num(); i++)
	{
		if (PlayersInsideTriggerVolume[i] == OtherActor)
		{
			PlayersInsideTriggerVolume.RemoveAt(i);
			CustomOverlapEndDelegate.Broadcast(OverlappedComponent, OtherActor);
			return;
		}
	}
}
