// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterFindFleeingLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Enemies/Hunter/HunterSpawnPoint.h"

EBTNodeResult::Type UBTT_HunterFindFleeingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	UWorld* World = GetWorld();
	if (World && OwnerComp.GetAIOwner())
	{
		AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(OwnerComp.GetAIOwner());
		if (UHunterSpawnPoint* RandFleeLoc = HunterController->GetRandHunterSpawnPoint())
		{
			BBComp->SetValueAsVector(FleeLocationKey.SelectedKeyName, RandFleeLoc->GetComponentLocation());
			Result = EBTNodeResult::Succeeded;
		}
	}
	
	return Result;
}
