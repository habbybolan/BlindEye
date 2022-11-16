// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "HunterTutorialSpawnPoint.generated.h"

UCLASS()
class BLINDEYE_API AHunterTutorialSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AHunterTutorialSpawnPoint();


	UPROPERTY()
	UArrowComponent* ArrowComponent;

};
