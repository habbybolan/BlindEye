// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Shrine.h"
#include "GameFramework/GameState.h"
#include "BlindEyeGameState.generated.h"

class ABlindEyeCharacter;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeGameState : public AGameState
{
	GENERATED_BODY()

public:

	ABlindEyeGameState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	AShrine* GetShrine();

protected:
	TWeakObjectPtr<AShrine> Shrine;

	
	
};
