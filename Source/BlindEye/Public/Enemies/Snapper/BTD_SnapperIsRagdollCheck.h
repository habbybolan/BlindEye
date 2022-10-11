// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_SnapperIsRagdollCheck.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTD_SnapperIsRagdollCheck : public UBTDecorator
{
	GENERATED_BODY()

	UBTD_SnapperIsRagdollCheck();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool IsRagdollCheck(UBehaviorTreeComponent& OwnerComp) const;
	
};
