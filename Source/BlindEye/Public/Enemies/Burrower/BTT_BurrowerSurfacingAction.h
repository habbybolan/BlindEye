// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerSurfacingAction.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerSurfacingAction : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerSurfacingAction();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
