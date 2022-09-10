// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTT_PerformBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_PerformBasicAttack : public UBTTask_Wait
{
	GENERATED_BODY()

public:

	UBTT_PerformBasicAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
