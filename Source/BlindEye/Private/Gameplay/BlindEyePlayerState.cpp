// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyePlayerState.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
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
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_PlayerDeathStateUpdated();
	}
}

bool ABlindEyePlayerState::GetIsTutorialFinished()
{
	return bFinishedTutorial;
}
 
void ABlindEyePlayerState::SetTutorialFinished(bool IsTutorialFinished)
{
	bFinishedTutorial = IsTutorialFinished;
}

void ABlindEyePlayerState::OnRep_HealthUpdated()
{
	ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetPawn());
	if (BlindEyeCharacter)
	{
		BlindEyeCharacter->HealthUpdated();
	}
}

void ABlindEyePlayerState::OnRep_PlayerDeathStateUpdated()
{
	UWorld* World = GetWorld();
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
	check(BlindEyeGS)
	if (IsDead)
	{
		PlayerDeathDelegate.Broadcast(this);
		BlindEyeGS->OnPlayerDied(this);
	} else
	{
		PlayerRevivedDelegate.Broadcast(this);
		BlindEyeGS->OnPlayerRevived(this);
	}
}

void ABlindEyePlayerState::OnRep_BirdMeterUpdated()
{
	ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetPawn());
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
