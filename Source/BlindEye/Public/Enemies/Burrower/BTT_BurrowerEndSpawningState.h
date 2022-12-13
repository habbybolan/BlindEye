// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BurrowerEndSpawningState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerEndSpawningState : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector StateKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EnemyActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
