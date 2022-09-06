// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlindEyePlayerController.generated.h"

class ABlindEyeCharacter;
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

private:
	UPROPERTY()
	ABlindEyeCharacter* OwningCharacter;
	
};

// TODO: Temporary for test spawning
static bool IsServer = true;
