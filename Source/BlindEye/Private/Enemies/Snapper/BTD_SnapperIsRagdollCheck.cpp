// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/BTD_SnapperIsRagdollCheck.h"

#include "AIController.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "Enemies/Snapper/SnapperEnemyController.h"

UBTD_SnapperIsRagdollCheck::UBTD_SnapperIsRagdollCheck()
{
	NodeName = TEXT("Is Ragdolling");
	bNotifyTick = true;
}

bool UBTD_SnapperIsRagdollCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{	
	return IsRagdollCheck(OwnerComp);
}

void UBTD_SnapperIsRagdollCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (IsRagdollCheck(OwnerComp))
	{
		//ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
		OwnerComp.RequestExecution(this);
		// ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
	}
}

bool UBTD_SnapperIsRagdollCheck::IsRagdollCheck(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(Controller))
	{
		if (ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(SnapperController->GetPawn()))
		{
			return Snapper->GetIsRagdolling();
		}
	}
	return false;
}
