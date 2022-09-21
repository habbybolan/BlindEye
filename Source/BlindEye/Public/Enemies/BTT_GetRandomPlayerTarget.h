// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_GetRandomPlayerTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_GetRandomPlayerTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard) 
	FBlackboardKeySelector EnemyActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
