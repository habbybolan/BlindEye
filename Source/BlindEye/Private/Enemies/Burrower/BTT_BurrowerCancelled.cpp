// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerCancelled.h"

#include "AIController.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerCancelled::UBTT_BurrowerCancelled()
{
	NodeName = "Cancel Hide";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_BurrowerCancelled::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
	BurrowerController->CancelHide();
	return EBTNodeResult::Succeeded;
}
