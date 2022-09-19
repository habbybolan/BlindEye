// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MoveToLocationImmediate.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_MoveToLocationImmediate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard) 
	FBlackboardKeySelector SelfKey;

	UPROPERTY(EditAnywhere, Category=Blackboard) 
	FBlackboardKeySelector LocationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
