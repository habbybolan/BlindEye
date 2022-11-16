// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "TutorialSpawnPoint.generated.h"

UCLASS()
class BLINDEYE_API ATutorialSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialSpawnPoint();

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ArrowComponent;

};
