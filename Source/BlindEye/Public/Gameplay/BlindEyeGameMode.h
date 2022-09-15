// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/GameMode.h"
#include "BlindEyeGameMode.generated.h"

class AHunterEnemyController;
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
	TArray<TSubclassOf<ABlindEyePlayerCharacter>> PlayerClassTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimerUntilGameWon = 60;

	// called by shrine when it's destroyed
	void OnShrineDeath();

	virtual void RestartGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemyController> HunterControllerType;

protected:
	virtual void BeginPlay() override;
	
	void OnGameEnded();
	void OnGameWon();

	FTimerHandle GameRestartTimerHandle;
	FTimerHandle GameWinTimerHandle;
	
	
};
