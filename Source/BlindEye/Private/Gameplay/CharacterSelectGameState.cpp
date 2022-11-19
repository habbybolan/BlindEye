// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/CharacterSelectGameState.h"

#include "Net/UnrealNetwork.h"

void ACharacterSelectGameState::OnRep_PhoenixPlayerSelected()
{
	// TODO:
}

void ACharacterSelectGameState::OnRep_CrowPlayerSelected()
{
	// TODO:
}

void ACharacterSelectGameState::UpdateReadyState()
{
	// TODO:
}

void ACharacterSelectGameState::SER_PlayerTrySelect_Implementation(EPlayerType PlayerType, ULocalPlayer* LocalPlayer)
{
	// TODO:
	if (PlayerType == EPlayerType::CrowPlayer)
	{
		if (CrowPlayer == nullptr && PhoenixPlayer != LocalPlayer)
		{
			CrowPlayer = LocalPlayer;
		} else return;
	} else
	{
		if (PhoenixPlayer == nullptr && CrowPlayer != LocalPlayer)
		{
			PhoenixPlayer = LocalPlayer;
		} else return;
	}

	UpdateReadyState();
}


void ACharacterSelectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterSelectGameState, PhoenixPlayer)
	DOREPLIFETIME(ACharacterSelectGameState, CrowPlayer)
}
