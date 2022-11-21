// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BurrowerFindProjectileTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerFindProjectileTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector ProjectileTargetKey;
	
};
