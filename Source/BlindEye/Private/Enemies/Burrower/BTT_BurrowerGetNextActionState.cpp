// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerGetNextActionState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

EBTNodeResult::Type UBTT_BurrowerGetNextActionState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
	BurrowerController->CalcNewActionState();
	UBlackboardComponent* BBComp =  OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsEnum(ActionStateKey.SelectedKeyName, (uint8)BurrowerController->GetCurrAction());
	return EBTNodeResult::Succeeded;
}
