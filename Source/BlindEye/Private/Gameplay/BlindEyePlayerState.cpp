// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyePlayerState.h"

#include "Characters/BlindEyeCharacter.h"
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

float ABlindEyePlayerState::GetMaxBirdMeter()
{
	return MaxBirdMeter;
}

void ABlindEyePlayerState::SetBirdMeter(float NewBirdMeter)
{
	CurrBirdMeter = NewBirdMeter;
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_BirdMeterUpdated();
	}
}

bool ABlindEyePlayerState::GetIsDead()
{
	return IsDead;
}

void ABlindEyePlayerState::SetIsDead(bool isDead)
{ 
	IsDead = isDead;
}

void ABlindEyePlayerState::OnRep_HealthUpdated()
{
	ABlindEyeCharacter* BlindEyeCharacter = Cast<ABlindEyeCharacter>(GetPawn());
	if (BlindEyeCharacter)
	{
		BlindEyeCharacter->HealthUpdated();
	}
}

void ABlindEyePlayerState::OnRep_BirdMeterUpdated()
{
	ABlindEyeCharacter* BlindEyeCharacter = Cast<ABlindEyeCharacter>(GetPawn());
	if (BlindEyeCharacter)
	{
		BlindEyeCharacter->BirdMeterUpdated();
	}
}

void ABlindEyePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyePlayerState, CurrHealth);
	DOREPLIFETIME(ABlindEyePlayerState, CurrBirdMeter);
	DOREPLIFETIME(ABlindEyePlayerState, IsDead);
}