// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/DummyEnemy.h"
#include "Tutorial/DummySpawnPoint.h"
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

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADummyEnemy> DummyType;

	UPROPERTY(EditDefaultsOnly)
	uint8 NumTimesForEachPlayerToDetonate = 8;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADummySpawnPoint> SpawnPointTypes;

	virtual void SetupTutorial() override;

	virtual void EndTutorialLogic() override;

	UFUNCTION()
	void OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType);

	virtual void PlayerEnteredTutorialHelper(ABlindEyePlayerCharacter* Player) override;

protected:
	
	uint8 NumCrowMarksToDetonate = 0;
	uint8 NumPhoenixMarksToDetonate = 0;

	bool bPhoenixPlayerFinished = false;
	bool bCrowPlayerFinished = false;

	void SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished);
	
};
