// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerSetNotAttackExpired.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_BurrowerSetNotAttackExpired::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsBool(IsAttackingExpiredKey.SelectedKeyName, false);
	return EBTNodeResult::Succeeded;
}
