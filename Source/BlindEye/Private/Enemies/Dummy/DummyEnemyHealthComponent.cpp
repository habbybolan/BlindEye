// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Dummy/DummyEnemyHealthComponent.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

UDummyEnemyHealthComponent::UDummyEnemyHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	IsInvincible = true;
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
		if (CurrMark.MarkerType == EMarkerType::Crow)
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::CrowPlayer))
			{
				Player->TryFinishTutorial(ETutorialChecklist::Detonate);
			}
		}
		// Phoenix player detonating dummy
		else
		{
			if (ABlindEyePlayerCharacter* Player = BlindEyeGS->GetPlayer(EPlayerType::PhoenixPlayer))
			{
				Player->TryFinishTutorial(ETutorialChecklist::Detonate);
			}
		}
	}
	
	Super::DetonateMark();
}
