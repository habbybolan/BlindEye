// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsInRangeOfTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTD_IsInRangeOfTarget : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AcceptableRadius = 50;

	UBTD_IsInRangeOfTarget();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
