// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerFindSpawningLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerFindSpawningLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerFindSpawningLocation();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector SpawningLocationKey;
	UPROPERTY(EditAnywhere, Category=Blackboard) 
	FBlackboardKeySelector SpawningRotationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
