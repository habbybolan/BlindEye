// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CharacterSelectPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACharacterSelectPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	bool bReady = false;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
