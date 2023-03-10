// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerGetNextActionState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerGetNextActionState::UBTT_BurrowerGetNextActionState()
{
	NodeName = TEXT("Calc Next Action State");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_BurrowerGetNextActionState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
	BurrowerController->CalcNewActionState();
	UBlackboardComponent* BBComp =  OwnerComp.GetBlackboardComponent();

	// TODO: FLip back to getting value from controller
	//BBComp->SetValueAsEnum(ActionStateKey.SelectedKeyName, (uint8)BurrowerController->GetCurrAction());
	BBComp->SetValueAsEnum(ActionStateKey.SelectedKeyName, (uint8)EBurrowActionState::Spawning);
	
	return EBTNodeResult::Succeeded;
}
