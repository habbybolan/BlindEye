// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Characters/CharacterSelectPlayerController.h"
#include "Characters/CharacterSelectPlayerState.h"
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
	ACharacterSelectPlayerState* PhoenixPlayer = nullptr;
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_CrowPlayerSelected")
	ACharacterSelectPlayerState* CrowPlayer = nullptr;

	// Local player trying to select a specific character
	void PlayerTrySelect(EPlayerType PlayerType, ACharacterSelectPlayerController* LocalPlayer);

	// Player readying up
	void PlayerTryReady(ACharacterSelectPlayerController* LocalPlayer);
	
	bool IsAllPlayersReady();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PhoenixPlayerSelected();
	UFUNCTION()
	void OnRep_CrowPlayerSelected();
	void PlayerSelected(EPlayerType PlayerType, APlayerState* PlayerThatSelected);

	void TrySelectHelper(EPlayerType PlayerType, APlayerState* PlayerThatActioned);
	bool IsPlayerSelectedCharacter(APlayerState* Player);

	ACharacterSelectPlayerController* GetOwnerPlayerController();
};
