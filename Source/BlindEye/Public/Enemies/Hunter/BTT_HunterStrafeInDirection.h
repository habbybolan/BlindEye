// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemyController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_MoveDirectlyToward.h"
#include "BTT_HunterStrafeInDirection.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_HunterStrafeInDirection : public UBTTask_MoveDirectlyToward
{
	GENERATED_BODY()

	UBTT_HunterStrafeInDirection(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	EStrafeDirection StrafeDirection;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
