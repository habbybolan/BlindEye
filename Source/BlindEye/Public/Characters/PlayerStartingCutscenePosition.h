// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerStartingCutscenePosition.generated.h"

UCLASS()
class BLINDEYE_API APlayerStartingCutscenePosition : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerStartingCutscenePosition();

	UPROPERTY()
	UArrowComponent* ArrowComponent;

};
