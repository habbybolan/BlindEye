// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DetonatingDummyEnemy.h"
#include "Tutorial/TutorialBase.h"
#include "DetonatingTutorial.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ADetonatingTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADetonatingDummyEnemy> DummyType;

	virtual void SetupTutorial() override;

	virtual void SetTutorialRunning() override;

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
