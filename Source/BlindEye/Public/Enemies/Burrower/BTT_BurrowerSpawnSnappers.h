// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerSpawnSnappers.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerSpawnSnappers : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerSpawnSnappers();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
