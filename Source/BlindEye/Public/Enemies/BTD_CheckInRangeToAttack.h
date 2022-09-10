// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BehaviorTree/Decorators/BTDecorator_BlueprintBase.h"
#include "BehaviorTree/Decorators/BTDecorator_DoesPathExist.h"
#include "BTD_CheckInRangeToAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTD_CheckInRangeToAttack : public UBTDecorator
{
	GENERATED_BODY()

public:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected: 

	UPROPERTY(EditAnywhere, Category=Condition)
	FBlackboardKeySelector Target;
 
	UPROPERTY(EditAnywhere)
	float DistanceToAttack = 200; 
	
};
