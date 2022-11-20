// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/CharacterSelectGameState.h"

#include "Characters/CharacterSelectPlayerController.h"
#include "GameFramework/PlayerState.h"
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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Cyan, "[CharacterSelectGameState:PlayerSelected]: Player selected");
		CharacterSelectPC->UpdatePlayerSelectedCharacter(PlayerType, PlayerThatSelected);
	}
}

void ACharacterSelectGameState::UpdateReadyState()
{
	// TODO:
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

	PlayerSelected(PlayerType, PlayerType == EPlayerType::CrowPlayer ? CrowPlayer : PhoenixPlayer);
	UpdateReadyState();
}


void ACharacterSelectGameState::TrySelectHelper(EPlayerType PlayerType, APlayerState* PlayerThatActioned)
{
	if (PlayerThatActioned == nullptr) return;
	APlayerState** PlayerReferenceToSelectedCharacter = PlayerType == EPlayerType::CrowPlayer ? &CrowPlayer : &PhoenixPlayer;
	if (*PlayerReferenceToSelectedCharacter == PlayerThatActioned)
	{
		*PlayerReferenceToSelectedCharacter = nullptr;
	}
	else if (*PlayerReferenceToSelectedCharacter == nullptr && !IsPlayerSelectedCharacter(PlayerThatActioned))
	{
		*PlayerReferenceToSelectedCharacter = PlayerThatActioned;
	}
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
