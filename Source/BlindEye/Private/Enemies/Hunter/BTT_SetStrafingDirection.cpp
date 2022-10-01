// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_SetStrafingDirection.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SetStrafingDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
	{
		BBComp->SetValueAsEnum(StrafingDirectionKey.SelectedKeyName, (uint8)StrafingDirection);
	}
	return EBTNodeResult::Succeeded;
}
