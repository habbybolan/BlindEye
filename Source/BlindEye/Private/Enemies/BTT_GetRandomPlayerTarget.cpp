// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_GetRandomPlayerTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UBTT_GetRandomPlayerTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return EBTNodeResult::Failed;
	
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
	{
		TArray<APlayerState*> PlayerList = GameState->PlayerArray;
		if (PlayerList.Num() == 0) return EBTNodeResult::Failed;
		
		APlayerState* RandPlayerState = PlayerList[UKismetMathLibrary::RandomIntegerInRange(0, PlayerList.Num() - 1)];
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		BBComp->SetValueAsObject(EnemyActorKey.SelectedKeyName, RandPlayerState->GetPawn());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
