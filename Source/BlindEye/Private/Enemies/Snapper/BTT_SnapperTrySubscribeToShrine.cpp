// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/BTT_SnapperTrySubscribeToShrine.h"

#include "AIController.h"
#include "Shrine.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SnapperTrySubscribeToShrine::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	
	UObject* ShrineObject = BBComp->GetValueAsObject(ShrineActorKey.SelectedKeyName);
	if (ShrineObject)
	{
		AShrine* Shrine = Cast<AShrine>(ShrineObject);

		AAIController* Controller = OwnerComp.GetAIOwner();
		if (Controller->GetPawn())
		{
			ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(Controller->GetPawn());
			// if already subscribed to a point, dont try subscribing again
			if (Snapper->GetShrineAttackPoint() != nullptr)
			{
				Result = EBTNodeResult::Succeeded;
			}
			else if (UShrineAttackPoint* AttackingPoint = Shrine->AskForClosestPoint(Snapper))
			{
				BBComp->SetValueAsObject(ShrineAttackPointKey.SelectedKeyName, AttackingPoint);
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	
	return Result;
}
