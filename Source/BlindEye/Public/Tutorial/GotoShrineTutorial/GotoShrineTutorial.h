// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HUD/BasicSingleIndicator.h"
#include "Tutorial/TutorialBase.h"
#include "GotoShrineTutorial.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API AGotoShrineTutorial : public ATutorialBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float CheckTutorialPlayersRadius = 500;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBasicSingleIndicator> GotoShrineIndicator;

	virtual void SetupTutorial() override;
	virtual void EndTutorial() override;

protected:

	FTimerHandle CheckPlayersNearTimerHandle;

	UPROPERTY()
	AActor* ShrineActor;
	
	UFUNCTION()
	void CheckPlayersNearbyHelper();
	void StartWaitingForPlayersToInteract();

	const FName GotoShrineIndicatorID = "GotoShrineIndicator";
	
};