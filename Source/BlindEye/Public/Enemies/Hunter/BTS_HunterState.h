// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "HunterEnemy.h"
#include "HunterEnemyController.h"
#include "BehaviorTree/BTService.h"
#include "BTS_HunterState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_HunterState : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsAttackingKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsImmobilizedKey; 

	UPROPERTY()
	AHunterEnemy* Hunter;

	UPROPERTY()
	AHunterEnemyController* HunterController;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector bChargedOnCooldownKey;

	UBTS_HunterState();

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
