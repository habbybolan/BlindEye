// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_HunterTryToAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_HunterTryToAttack : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector EnemyActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
