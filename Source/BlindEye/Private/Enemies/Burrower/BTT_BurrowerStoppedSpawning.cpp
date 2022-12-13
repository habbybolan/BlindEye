// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerStoppedSpawning.h"

#include "Enemies/Burrower/BurrowerEnemyController.h"

EBTNodeResult::Type UBTT_BurrowerStoppedSpawning::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->NotifySpawningStopped();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
