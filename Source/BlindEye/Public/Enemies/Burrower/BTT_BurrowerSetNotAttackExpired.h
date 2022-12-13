// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BurrowerSetNotAttackExpired.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerSetNotAttackExpired : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsAttackingExpiredKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
