// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_HunterTargetMarked.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTD_HunterTargetMarked : public UBTDecorator
{
	GENERATED_BODY()

	UBTD_HunterTargetMarked();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EnemyActorKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
