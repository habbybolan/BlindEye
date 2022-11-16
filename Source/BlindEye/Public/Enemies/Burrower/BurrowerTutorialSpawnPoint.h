// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "BurrowerTutorialSpawnPoint.generated.h"

UCLASS()
class BLINDEYE_API ABurrowerTutorialSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ABurrowerTutorialSpawnPoint();

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ArrowComponent;

};
