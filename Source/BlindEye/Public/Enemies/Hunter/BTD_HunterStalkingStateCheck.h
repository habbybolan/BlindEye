// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTD_HunterStalkingStateCheck.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTD_HunterStalkingStateCheck : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

	UBTD_HunterStalkingStateCheck();

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector bDamagedKey;
 
	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector bStalkingStrafeKey;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
