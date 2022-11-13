// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "TutorialManager.h"
#include "GameFramework/Actor.h"
#include "TutorialBase.generated.h"

class ABlindEyeGameState;

UCLASS(Abstract)
class BLINDEYE_API ATutorialBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialBase();
 
	virtual void SetupTutorial();
	virtual void SetTutorialRunning() PURE_VIRTUAL(ATutorialBase::SetTutorialRunning,);
	virtual void EndTutorial();

	bool bRunning = false;
 
	UPROPERTY()
	ABlindEyeGameState* BlindEyeGS;
};
