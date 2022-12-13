// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerTriggerVolume.h"

#include "Kismet/KismetSystemLibrary.h"

UBurrowerTriggerVolume::UBurrowerTriggerVolume()
{
	SetGenerateOverlapEvents(true);
}

void UBurrowerTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UBurrowerTriggerVolume::OnBeginOverlapHelper);
	OnComponentEndOverlap.AddDynamic(this, &UBurrowerTriggerVolume::OnEndOverlapHelper);

	UWorld* World = GetWorld();
	if (World)
	{
		World->OnWorldBeginPlay.AddUFunction(this, TEXT("Initialize"));
	}
}

void UBurrowerTriggerVolume::Initialize()
{
	CheckOverlappingPlayersOnSpawn();
}

TArray<ABlindEyePlayerCharacter*> UBurrowerTriggerVolume::GetPlayerActorsOverlapping()
{
	return PlayersInsideTriggerVolume;
}

void UBurrowerTriggerVolume::OnBeginOverlapHelper(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		PlayersInsideTriggerVolume.AddUnique(Player);
	}
	CustomOverlapStartDelegate.Broadcast(OverlappedComponent, OtherActor);
}

void UBurrowerTriggerVolume::OnEndOverlapHelper(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(OtherActor))
	{
		for (uint8 i = 0; i < PlayersInsideTriggerVolume.Num(); i++)
		{
			if (PlayersInsideTriggerVolume[i] == OtherActor)
			{
				PlayersInsideTriggerVolume.RemoveAt(i);
			
			}
		}
	}
	
	CustomOverlapEndDelegate.Broadcast(OverlappedComponent, OtherActor);
}

void UBurrowerTriggerVolume::CheckOverlappingPlayersOnSpawn()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ABlindEyePlayerCharacter::StaticClass());
	for (AActor* PlayerActor : OverlappingActors)
	{
		PlayersInsideTriggerVolume.Add(Cast<ABlindEyePlayerCharacter>(PlayerActor));
	}
}
