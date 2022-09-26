// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_SetHunterState.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SetHunterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsEnum(HunterStateKey.SelectedKeyName, (uint8)NextHunterState);

	// Update speed based on new state
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(OwnerComp.GetAIOwner()))
	{
		HunterController->UpdateMovementSpeed(NextHunterState);
	}
	
	return EBTNodeResult::Succeeded;
}
