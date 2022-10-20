// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Engine/TriggerVolume.h"
#include "BurrowerTriggerVolume.generated.h"

enum class EIslandPosition : uint8;

UCLASS()
class BLINDEYE_API ABurrowerTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:

	ABurrowerTriggerVolume();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	EIslandPosition IslandType;

	UPROPERTY()
	TArray<ABlindEyePlayerCharacter*> PlayersInsideTriggerVolume;

	TArray<ABlindEyePlayerCharacter*> GetPlayerActorsOverlapping();

	// Call this for ending overlap so this class can process overlap ending before broadcasting
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOverlapEndSignature, AActor*, AActor*);
	FOverlapEndSignature CustomOverlapDelegate; 

	UFUNCTION()
	void OnPlayerOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
};
