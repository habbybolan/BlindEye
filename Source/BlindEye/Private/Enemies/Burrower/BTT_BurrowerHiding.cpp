// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerHiding.h"

#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerHiding::UBTT_BurrowerHiding()
{
	NodeName = TEXT("Hiding");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_BurrowerHiding::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->StartHiding();
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UBTT_BurrowerHiding::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		if (!BurrowerController->IsHiding())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	} else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
