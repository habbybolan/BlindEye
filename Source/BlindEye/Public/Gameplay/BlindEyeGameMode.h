// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyeCharacter.h"
#include "Enemies/HunterEnemyController.h"
#include "GameFramework/GameMode.h"
#include "BlindEyeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	FTransform GetSpawnPoint() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<ABlindEyeCharacter>> PlayerClassTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AHunterEnemyController> HunterControllerType;

	// called by shrine when it's destroyed
	void OnShrineDeath();

protected:
	virtual void BeginPlay() override;
	
	void OnGameEnded();

	FTimerHandle GameRestartTimerHandle;
	virtual void RestartGame() override;
	
};
