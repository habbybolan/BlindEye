// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Tutorial/TutorialSpawnPoint.h"
#include "DummySpawnPoint.generated.h"

UENUM()
enum class EDummySpawnType : uint8
{
	Crow,
	Phoenix,
	General
};

/**
 * Holding all dummy spawns during the tutorial phase
 */
UCLASS()
class BLINDEYE_API ADummySpawnPoint : public ATutorialSpawnPoint
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	EDummySpawnType DummyType;
	
};
