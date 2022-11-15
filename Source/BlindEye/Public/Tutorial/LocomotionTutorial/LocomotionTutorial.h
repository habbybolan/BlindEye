// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/TutorialBase.h"
#include "LocomotionTutorial.generated.h"

struct FFinishedTasks
{
	bool bMovement = false;

	uint8 CurrJumpCount = 0;
	uint8 MaxJumpCount = 1;

	uint8 CurrDashCount = 0;
	uint8 MaxDashCount = 1;

	bool IsFinished()
	{
		return bMovement && CurrJumpCount >= MaxJumpCount && CurrDashCount >= MaxDashCount;
	}
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ALocomotionTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	uint8 JumpCount = 2;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	uint8 DashCount = 2;
	
	virtual void SetupTutorial() override;
	virtual void EndTutorial() override;

protected:

	TArray<FFinishedTasks> PlayersFinishedTasks;

	void SetupCheckboxes(EPlayerType PlayerType);

	UFUNCTION()
	void OnPlayerAction(ABlindEyePlayerCharacter* Player, TutorialInputActions::ETutorialInputActions InputActions);

	void CheckAllPlayersFinished();
};
