// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlindEyePlayerController.generated.h"

class ABlindEyePlayerCharacter;
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

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_RestartLevel();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_CharacterSelect();

	virtual void OnPossess(APawn* InPawn) override;

	FRotator GetDesiredRotationFromMouse();

private:
	UPROPERTY()
	ABlindEyePlayerCharacter* OwningCharacter;
	
};

// TODO: Temporary for test spawning
static bool IsServer = true;
