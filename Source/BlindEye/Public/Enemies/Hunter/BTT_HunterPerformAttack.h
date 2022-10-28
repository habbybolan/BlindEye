// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_HunterPerformAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_HunterPerformAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_HunterPerformAttack();

	UPROPERTY(EditAnywhere) 
	EHunterAttacks AttackType;

	UPROPERTY()
	AHunterEnemy* Hunter;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
	
};
