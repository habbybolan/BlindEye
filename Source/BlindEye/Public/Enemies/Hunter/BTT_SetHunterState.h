// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemyController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetHunterState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_SetHunterState : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector HunterStateKey;

	UPROPERTY(EditAnywhere)
	EHunterStates NextHunterState;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
