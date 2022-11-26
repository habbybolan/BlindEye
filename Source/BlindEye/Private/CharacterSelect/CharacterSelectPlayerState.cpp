// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectPlayerState.h"

#include "Net/UnrealNetwork.h"

void ACharacterSelectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterSelectPlayerState, bReady);
}