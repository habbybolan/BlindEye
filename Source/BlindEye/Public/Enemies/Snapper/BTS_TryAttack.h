// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "SnapperEnemy.h"
#include "BehaviorTree/BTService.h"
#include "BTS_TryAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_TryAttack : public UBTService
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	ESnapperAttacks SnapperAttack;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector TargetKey;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
