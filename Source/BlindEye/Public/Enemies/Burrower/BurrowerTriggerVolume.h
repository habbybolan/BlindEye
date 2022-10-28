// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/TriggerVolume.h"
#include "BurrowerTriggerVolume.generated.h"

enum class EIslandPosition : uint8;

UCLASS()
class BLINDEYE_API UBurrowerTriggerVolume : public UBoxComponent
{
	GENERATED_BODY()

public:

	UBurrowerTriggerVolume();

	virtual void BeginPlay() override;
 
	UPROPERTY(EditAnywhere)
	uint8 IslandID;

	UPROPERTY()
	TArray<ABlindEyePlayerCharacter*> PlayersInsideTriggerVolume;

	TArray<ABlindEyePlayerCharacter*> GetPlayerActorsOverlapping();
 
	// Call this for ending overlap so this class can process overlap ending before broadcasting
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOverlapEndSignature, UPrimitiveComponent*, OverlappedActor, AActor*, OtherActor);
	FOverlapEndSignature CustomOverlapEndDelegate;
  
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOverlapStartSignature, UPrimitiveComponent*, OverlappedActor, AActor*, OtherActor);
	FOverlapStartSignature CustomOverlapStartDelegate; 

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
