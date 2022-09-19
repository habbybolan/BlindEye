// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerFindSpawningLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerFindSpawningLocation::UBTT_BurrowerFindSpawningLocation()
{
	NodeName = TEXT("Find Spawning Location");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_BurrowerFindSpawningLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		FTransform SpawnLocationTransform = BurrowerController->FindRandSpawnPoint();
		BBComp->SetValueAsVector(SpawningLocationKey.SelectedKeyName, SpawnLocationTransform.GetLocation());
		BBComp->SetValueAsRotator(SpawningRotationKey.SelectedKeyName, SpawnLocationTransform.Rotator());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
