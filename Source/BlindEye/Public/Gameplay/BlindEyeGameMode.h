// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyeCharacter.h"
#include "GameFramework/GameMode.h"
#include "BlindEyeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	FTransform GetSpawnPoint() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<ABlindEyeCharacter>> PlayerClassTypes;

	// called by shrine when it's destroyed
	void OnShrineDeath();

	virtual void RestartGame();

protected:
	void OnGameEnded();

	FTimerHandle GameRestartTimerHandle;
	
	
};
