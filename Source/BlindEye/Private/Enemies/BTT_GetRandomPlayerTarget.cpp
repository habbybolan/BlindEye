// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_GetRandomPlayerTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UBTT_GetRandomPlayerTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* ValidPlayer = FindValidPlayer();
	
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsObject(EnemyActorKey.SelectedKeyName, ValidPlayer);
	return EBTNodeResult::Succeeded;
}

AActor* UBTT_GetRandomPlayerTarget::FindValidPlayer()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return nullptr;
	
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
	{
		TArray<APlayerState*> PlayerList = GameState->PlayerArray;
		if (PlayerList.Num() == 0) return nullptr;
		uint8 randPlayerIndex = UKismetMathLibrary::RandomIntegerInRange(0, PlayerList.Num() - 1);

		// If rand player not valid, loop through until valid player found
		if (!CheckPlayerValid(PlayerList[randPlayerIndex]))
		{
			for (APlayerState* PlayerState : PlayerList)
			{
				if (CheckPlayerValid(PlayerState)) return PlayerState->GetPawn();
			}
		}
		// otherwise, player valid and set as target
		else
		{
			return PlayerList[randPlayerIndex]->GetPawn();
		}
	}
	return nullptr;
}

bool UBTT_GetRandomPlayerTarget::CheckPlayerValid(APlayerState* PlayerState)
{
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
	check(Player);
	return !Player->GetIsDead();
}
