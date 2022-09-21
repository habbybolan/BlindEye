// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerSurfacingAction.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerSurfacingAction::UBTT_BurrowerSurfacingAction()
{
	NodeName = TEXT("Surfacing");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_BurrowerSurfacingAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->StartSurfacing();
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UBTT_BurrowerSurfacingAction::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		if (!BurrowerController->IsSurfacing())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	} else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
