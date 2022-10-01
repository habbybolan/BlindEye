// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemyController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetStrafingDirection.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_SetStrafingDirection : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EStrafeDirection StrafingDirection; 

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector StrafingDirectionKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
