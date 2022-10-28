// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterStartChanneling.h"

#include "AIController.h"
#include "Enemies/Hunter/HunterEnemyController.h"

EBTNodeResult::Type UBTT_HunterStartChanneling::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	check(HunterController)

	HunterController->StartChanneling();
	return EBTNodeResult::Succeeded;
}
