// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HunterSpawnPoint.generated.h"


class UArrowComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UHunterSpawnPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	UHunterSpawnPoint();

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	uint8 IslandID;
		
};
