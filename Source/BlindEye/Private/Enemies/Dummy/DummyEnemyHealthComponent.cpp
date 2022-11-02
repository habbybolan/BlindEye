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
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		// TODO:
	}
	
	Super::DetonateMark();
}
