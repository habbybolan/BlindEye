// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Characters/CharacterSelectPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "CharacterSelectGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ACharacterSelectGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_PhoenixPlayerSelected")
	ULocalPlayer* PhoenixPlayer = nullptr;
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_CrowPlayerSelected")
	ULocalPlayer* CrowPlayer = nullptr;

	// Local player trying to select a specific character
	void PlayerTrySelect(EPlayerType PlayerType, ULocalPlayer* LocalPlayer);
	

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PhoenixPlayerSelected();
	UFUNCTION()
	void OnRep_CrowPlayerSelected();
	void PlayerSelected(EPlayerType PlayerType, ULocalPlayer* PlayerThatSelected);

	void UpdateReadyState();

	UFUNCTION(Server, Reliable)
	void SER_TrySelectHelper(EPlayerType PlayerType, ULocalPlayer* PlayerThatActioned);
	bool IsPlayerSelectedCharacter(ULocalPlayer* Player);

	ACharacterSelectPlayerController* GetOwnerPlayerController();
};
