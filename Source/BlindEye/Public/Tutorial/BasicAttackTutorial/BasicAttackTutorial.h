// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/TutorialBase.h"
#include "BasicAttackTutorial.generated.h"

struct FFinishedTasks_BasicAttack
{
	bool bBasicAttack = false;

	uint8 CurrComboCount = 0;
	uint8 MaxComboCount = 1;

	bool IsFinished()
	{
		return bBasicAttack && CurrComboCount >= MaxComboCount;
	}
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABasicAttackTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	uint8 ComboCount = 2;
	
	virtual void SetupTutorial() override;
	virtual void EndTutorial() override;

protected:

	TArray<FFinishedTasks_BasicAttack> PlayersFinishedTasks;

	void SetupCheckboxes(EPlayerType PlayerType);

	UFUNCTION()
	void OnPlayerAction(ABlindEyePlayerCharacter* Player, TutorialInputActions::ETutorialInputActions InputActions);

	void CheckAllPlayersFinished();
	
};
