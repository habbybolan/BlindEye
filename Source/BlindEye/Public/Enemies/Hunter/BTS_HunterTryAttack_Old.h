// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_HunterTryAttack_Old.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_HunterTryAttack_Old : public UBTService
{
	GENERATED_BODY()

public:

	UBTS_HunterTryAttack_Old();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector TargetKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
