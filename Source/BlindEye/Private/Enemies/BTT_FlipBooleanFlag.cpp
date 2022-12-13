// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_FlipBooleanFlag.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_FlipBooleanFlag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	bool currFlagVal = BBComp->GetValueAsBool(FlagKey.SelectedKeyName);
	BBComp->SetValueAsBool(FlagKey.SelectedKeyName, !currFlagVal);
	return EBTNodeResult::Succeeded;
}
