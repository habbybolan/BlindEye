// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Dummy/DummyEnemyHealthComponent.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

UDummyEnemyHealthComponent::UDummyEnemyHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	IsInvincible = false;
}

void UDummyEnemyHealthComponent::DetonateMark()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Check off detonate from checklist
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		// Crow player detonating dummy
		if (CurrMark.MarkerType == EMarkerType::Phoenix)
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::CrowPlayer))
			{
				Player->CLI_TryFinishTutorial(ETutorialChecklist::Detonate);
			}
		}
		// Phoenix player detonating dummy
		else
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::PhoenixPlayer))
			{
				Player->CLI_TryFinishTutorial(ETutorialChecklist::Detonate);
			}
		}
	}
	
	Super::DetonateMark();
}

void UDummyEnemyHealthComponent::AddMarkerHelper(EMarkerType MarkerType)
{
	Super::AddMarkerHelper(MarkerType);

	UWorld* World = GetWorld();
	if (World)
	{
		// Check off Adding mark from checklist
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		// Crow player marking dummy
		if (CurrMark.MarkerType == EMarkerType::Crow)
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::CrowPlayer))
			{
				Player->CLI_TryFinishTutorial(ETutorialChecklist::MarkEnemy);
			}
		}
		// Phoenix player marking dummy
		else
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::PhoenixPlayer))
			{
				Player->CLI_TryFinishTutorial(ETutorialChecklist::MarkEnemy);
			}
		}
	}
}

void UDummyEnemyHealthComponent::OnDeath(AActor* ActorThatKilled)
{
	// Do nothing
}
