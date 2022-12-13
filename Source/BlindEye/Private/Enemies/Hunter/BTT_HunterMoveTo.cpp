// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterMoveTo.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_HunterMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (Result != EBTNodeResult::InProgress)
	{
		return Result;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		if (BlackboardComp->GetValueAsBool(bCharged.SelectedKeyName))
		{
			FilterClass = LargeJumpQF;
		} else
		{
			FilterClass = SmallJumpQF;
		}
		return Result;
	}
	return EBTNodeResult::Failed;
}
