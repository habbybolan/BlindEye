// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTT_HunterMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_HunterMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNavigationQueryFilter> SmallJumpQF;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNavigationQueryFilter> LargeJumpQF;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector bChargedAttackOnCooldownKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
