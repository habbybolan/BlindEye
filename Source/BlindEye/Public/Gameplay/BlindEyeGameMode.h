// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
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
	
};
