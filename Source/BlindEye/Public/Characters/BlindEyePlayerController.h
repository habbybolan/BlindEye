// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlindEyePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable)
	void SER_SpawnPlayer();
	
};
