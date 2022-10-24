// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BurrowerCancelled.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerCancelled : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_BurrowerCancelled();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
