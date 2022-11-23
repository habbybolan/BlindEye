// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DetonatingDummyEnemy.h"
#include "Tutorial/TutorialBase.h"
#include "DetonatingTutorial.generated.h"

class ADummySpawnPoint;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ADetonatingTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADetonatingDummyEnemy> DummyType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADummySpawnPoint> SpawnPointTypes;

	virtual void SetupTutorial() override;

	virtual void EndTutorial() override;

	UFUNCTION()
	void OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType);

	virtual void PlayerEnteredTutorial(ABlindEyePlayerCharacter* Player) override;

protected:
	
	uint8 NumCrowMarksToDetonate = 0;
	uint8 NumPhoenixMarksToDetonate = 0;

	bool bPhoenixPlayerFinished = false;
	bool bCrowPlayerFinished = false;

	void SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished);
};
