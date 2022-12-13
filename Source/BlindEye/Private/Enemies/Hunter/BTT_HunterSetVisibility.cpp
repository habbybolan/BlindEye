// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterSetVisibility.h"

#include "AIController.h"
#include "Enemies/Hunter/HunterEnemyController.h"

EBTNodeResult::Type UBTT_HunterSetVisibility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller))
	{
		HunterController->TrySetVisibility(TurnIsVisible);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
