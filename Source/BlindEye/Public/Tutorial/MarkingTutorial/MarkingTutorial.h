// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/TutorialBase.h"
#include "MarkingTutorial.generated.h"

class ADummySpawnPoint;
enum class EMarkerType : uint8;
enum class EPlayerType : uint8;
class AMarkingDummyEnemy;

/**
 * 
 */
UCLASS()
class BLINDEYE_API AMarkingTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMarkingDummyEnemy> DummyType;

	UPROPERTY(EditDefaultsOnly)
	uint8 NumTimesForEachPlayerToMark = 8;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADummySpawnPoint> SpawnPointTypes;
	
	virtual void SetupTutorial() override;
	virtual void EndTutorialLogic() override;
	virtual void EndTutorial() override;

	virtual void PlayerEnteredTutorialHelper(ABlindEyePlayerCharacter* Player) override;

protected:

	uint8 NumDummiesForCrowToMark = 0;
	uint8 NumDummiesForPhoenixToMark = 0;

	bool bPhoenixPlayerFinished = false;
	bool bCrowPlayerFinished = false;
	
	UFUNCTION()
	void OnDummyMarked(AActor* MarkedPawn, EMarkerType MarkerType);

	void SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished);
};
