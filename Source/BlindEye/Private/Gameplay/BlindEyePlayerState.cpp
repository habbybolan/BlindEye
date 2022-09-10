// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyePlayerState.h"

#include "Net/UnrealNetwork.h"

ABlindEyePlayerState::ABlindEyePlayerState()
{
	CurrHealth = MaxHealth;
	CurrBirdMeter = MaxBirdMeter;
}

float ABlindEyePlayerState::GetHealth()
{
	return CurrHealth;
}

void ABlindEyePlayerState::SetHealth(float NewHealth)
{
	CurrHealth = NewHealth;
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_HealthUpdated();
	}
}

float ABlindEyePlayerState::GetMaxHealth()
{
	return MaxHealth;
}

float ABlindEyePlayerState::GetBirdMeter()
{
	return CurrBirdMeter;
}

void ABlindEyePlayerState::SetBirdMeter(float NewBirdMeter)
{
	CurrBirdMeter = NewBirdMeter;
}

void ABlindEyePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyePlayerState, CurrHealth);
	DOREPLIFETIME(ABlindEyePlayerState, CurrBirdMeter);
}

void ABlindEyePlayerState::OnRep_HealthUpdated()
{
	HealthUpdated.Broadcast();
}

void ABlindEyePlayerState::OnRep_BirdMeterUpdated()
{
	// TODO:
}
