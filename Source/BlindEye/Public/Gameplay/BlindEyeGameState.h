// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Shrine.h"
#include "GameFramework/GameState.h"
#include "BlindEyeGameState.generated.h"

class ABlindEyePlayerCharacter;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ABlindEyeGameState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	AShrine* GetShrine();
	
	UPROPERTY(Replicated)
	bool bWinConditionPaused = false;

	// debugger flag for making hunter visible
	UPROPERTY(Replicated)
	bool bHunterAlwaysVisible = false;

protected:
	TWeakObjectPtr<AShrine> Shrine;

	
	
	
};
