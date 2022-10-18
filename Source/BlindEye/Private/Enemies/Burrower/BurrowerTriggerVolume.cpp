// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerTriggerVolume.h"

ABurrowerTriggerVolume::ABurrowerTriggerVolume()
{
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void ABurrowerTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ABurrowerTriggerVolume::OnPlayerOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABurrowerTriggerVolume::OnPlayerEndOverlap);
}

TArray<ABlindEyePlayerCharacter*> ABurrowerTriggerVolume::GetPlayerActorsOverlapping()
{
	return PlayersInsideTriggerVolume;
}

void ABurrowerTriggerVolume::OnPlayerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		PlayersInsideTriggerVolume.AddUnique(Player);
	}
}

void ABurrowerTriggerVolume::OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	for (uint8 i = 0; i < PlayersInsideTriggerVolume.Num(); i++)
	{
		if (PlayersInsideTriggerVolume[i] == OtherActor)
		{
			PlayersInsideTriggerVolume.RemoveAt(i);
			CustomOverlapDelegate.Broadcast(OverlappedActor, OtherActor);
			return;
		}
	}
}
