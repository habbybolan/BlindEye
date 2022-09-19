// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerSpawnSnappers.h"

#include "AIController.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerSpawnSnappers::UBTT_BurrowerSpawnSnappers()
{
	NodeName = TEXT("Spawn Snappers");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_BurrowerSpawnSnappers::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->SpawnSnappers();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
