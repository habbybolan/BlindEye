// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerUpdateState.h"

#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerUpdateState::UBTT_BurrowerUpdateState()
{
	NodeName = TEXT("Update Visibility State");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_BurrowerUpdateState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AAIController* Controller = OwnerComp.GetAIOwner();
	// if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	// {
	// 	BurrowerController->SetBurrowerState(bHidden, bFollowing);
	// 	return EBTNodeResult::Succeeded;
	// }
	return EBTNodeResult::Failed;
}
