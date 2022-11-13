// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class ATutorialBase;

UCLASS()
class BLINDEYE_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialManager();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATutorialBase>> TutorialTypes;

	void StartTutorials();
	void GotoNextTutorial();

protected:

	UPROPERTY()
	TArray<ATutorialBase*> AllTutorials;

	uint8 CurrTutorialIndex = 0;
	

};
