// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "CharacterSelect/CharacterSelectPlayerController.h"
#include "CharacterSelect/CharacterSelectPlayerState.h"
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

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_PhoenixPlayerSelected")
	ACharacterSelectPlayerState* PhoenixPlayer = nullptr;
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_CrowPlayerSelected")
	ACharacterSelectPlayerState* CrowPlayer = nullptr;

	// Local player trying to select a specific character
	void PlayerTrySelect(EPlayerType PlayerType, ACharacterSelectPlayerController* LocalPlayer);

	// Player readying up
	UFUNCTION(Server, Reliable)
	void SER_PlayerTryReady(ACharacterSelectPlayerController* LocalPlayer);
	
	bool IsAllPlayersReady();

	void OnPlayerChanged(bool bJoined, AController* ChangedController);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PhoenixPlayerSelected();
	UFUNCTION()
	void OnRep_CrowPlayerSelected();
	void PlayerSelected(EPlayerType PlayerType, APlayerState* PlayerThatSelected);
	void PlayerReadyStateChanged(EPlayerType PlayerType, bool bReady);

	void TrySelectHelper(EPlayerType PlayerType, APlayerState* PlayerThatActioned);
	bool IsPlayerSelectedCharacter(APlayerState* Player);

	UPROPERTY()
	ACharacterSelectModel* CrowModel;
	UPROPERTY()
	ACharacterSelectModel* PhoenixModel; 

	ACharacterSelectPlayerController* GetOwnerPlayerController();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnPlayerChanged(bool bJoined, AController* ChangedController);
};
