// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemy.h"
#include "Tutorial/TutorialBase.h"
#include "DetonateAlliesMarkTutorial.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ADetonateAlliesMarkTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADummyEnemy> DummyType;

	UPROPERTY(EditDefaultsOnly)
	uint8 NumTimesForEachPlayerToDetonate = 8;

	virtual void SetupTutorial() override;

	virtual void EndTutorial() override;

	UFUNCTION()
	void OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType);

protected:
	
	uint8 NumCrowMarksToDetonate = 0;
	uint8 NumPhoenixMarksToDetonate = 0;

	bool bPhoenixPlayerFinished = false;
	bool bCrowPlayerFinished = false;

	void SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished);
	
};
