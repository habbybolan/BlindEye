// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_HunterTryAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_HunterTryAttack : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector EnemyActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
