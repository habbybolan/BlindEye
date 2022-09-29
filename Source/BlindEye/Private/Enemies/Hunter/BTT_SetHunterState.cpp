// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_SetHunterState.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SetHunterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsEnum(HunterStateKey.SelectedKeyName, (uint8)NextHunterState);

	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(OwnerComp.GetAIOwner());
	if (HunterController == nullptr) return EBTNodeResult::Failed;
	
	// Update speed based on new state
	HunterController->UpdateMovementSpeed(NextHunterState);
	
	switch (NextHunterState)
	{
	case EHunterStates::Attacking:
		HunterController->TrySetVisibility(true);
		break;
	case EHunterStates::Stalking:
		HunterController->TrySetVisibility(false);
		BBComp->SetValueAsBool(bDamagedKey.SelectedKeyName, false);
		BBComp->SetValueAsEnum(StrafingDirection.SelectedKeyName, (uint8)EStrafeDirection::Left);
		break;
	case EHunterStates::Running:
		HunterController->TrySetVisibility(false);
		break;
	}
	
	
	return EBTNodeResult::Succeeded;
}
