// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Net/UnrealNetwork.h"

ABlindEyeGameState::ABlindEyeGameState()
{
}

void ABlindEyeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
