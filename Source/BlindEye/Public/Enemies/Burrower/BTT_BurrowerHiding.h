// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerHiding.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerHiding : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerHiding();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
