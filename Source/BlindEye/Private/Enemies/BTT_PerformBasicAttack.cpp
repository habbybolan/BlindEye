// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_PerformBasicAttack.h"

UBTT_PerformBasicAttack::UBTT_PerformBasicAttack()
{
	NodeName = "Basic Attack";
}

EBTNodeResult::Type UBTT_PerformBasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Red, "Try Attack");
	return EBTNodeResult::Succeeded;
}
