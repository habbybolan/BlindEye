// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/CharacterSelectGameState.h"

#include "Characters/CharacterSelectPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameInstance.h"
#include "Net/UnrealNetwork.h"

void ACharacterSelectGameState::OnRep_PhoenixPlayerSelected()
{
	PlayerSelected(EPlayerType::PhoenixPlayer, PhoenixPlayer);
}

void ACharacterSelectGameState::OnRep_CrowPlayerSelected()
{
	PlayerSelected(EPlayerType::CrowPlayer, CrowPlayer);
}

void ACharacterSelectGameState::PlayerSelected(EPlayerType PlayerType, APlayerState* PlayerThatSelected)
{
	if (ACharacterSelectPlayerController* CharacterSelectPC = GetOwnerPlayerController())
	{
		CharacterSelectPC->UpdatePlayerSelectedCharacter(PlayerType, PlayerThatSelected);
	}
}

ACharacterSelectPlayerController* ACharacterSelectGameState::GetOwnerPlayerController()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			return Cast<ACharacterSelectPlayerController>(PlayerController);
		}
	}
	return nullptr;
}
 
void ACharacterSelectGameState::PlayerTrySelect(EPlayerType PlayerType, ACharacterSelectPlayerController* ControllerThatSelected)
{
	// Update the character the has selected/unselected current character
	if (PlayerType == EPlayerType::CrowPlayer)
	{
		TrySelectHelper(PlayerType, ControllerThatSelected->PlayerState);
	} else
	{
		TrySelectHelper(PlayerType, ControllerThatSelected->PlayerState);
	}
}

void ACharacterSelectGameState::TrySelectHelper(EPlayerType PlayerType, APlayerState* PlayerThatActioned)
{
	if (PlayerThatActioned == nullptr) return;
	ACharacterSelectPlayerState** PlayerReferenceToSelectedCharacter = PlayerType == EPlayerType::CrowPlayer ? &CrowPlayer : &PhoenixPlayer;
	// Unselecting character
	if (*PlayerReferenceToSelectedCharacter == PlayerThatActioned)
	{
		*PlayerReferenceToSelectedCharacter = nullptr;
		PlayerSelected(PlayerType, nullptr);
	}
	// Selecting character
	else if (*PlayerReferenceToSelectedCharacter == nullptr && !IsPlayerSelectedCharacter(PlayerThatActioned))
	{
		*PlayerReferenceToSelectedCharacter = Cast<ACharacterSelectPlayerState>(PlayerThatActioned);
		PlayerSelected(PlayerType, *PlayerReferenceToSelectedCharacter);
	}
}

void ACharacterSelectGameState::PlayerTryReady(ACharacterSelectPlayerController* PlayerReadied)
{
	bool bPlayerReadied = false;
	// Ready up player if valid
	if (PlayerReadied->PlayerState)
	{
		if (IsPlayerSelectedCharacter(PlayerReadied->PlayerState))
		{
			ACharacterSelectPlayerState* CharacterSelectPS = Cast<ACharacterSelectPlayerState>(PlayerReadied->PlayerState);
			CharacterSelectPS->bReady = true;
			bPlayerReadied = true;
		}
	}

	if (bPlayerReadied)
	{
		if (IsAllPlayersReady())
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green,
				TEXT("[ACharacterSelectGameState::PlayerTryReady] CrowID: " + CrowPlayer->GetUniqueId().ToString() +
					" PhoenixID: " + PhoenixPlayer->GetUniqueId().ToString()));
			UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
			BlindEyeGI->EnterGame(CrowPlayer->GetUniqueId().ToString(), PhoenixPlayer->GetUniqueId().ToString());
		}
		else
		{
			// TODO: Notify Players that player is waiting/ready
		}
	}
}

bool ACharacterSelectGameState::IsAllPlayersReady()
{
	return CrowPlayer != nullptr && CrowPlayer->bReady &&
		PhoenixPlayer != nullptr && PhoenixPlayer->bReady;
}

bool ACharacterSelectGameState::IsPlayerSelectedCharacter(APlayerState* Player)
{
	return CrowPlayer == Player || PhoenixPlayer == Player;
}

void ACharacterSelectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterSelectGameState, PhoenixPlayer)
	DOREPLIFETIME(ACharacterSelectGameState, CrowPlayer)
}
