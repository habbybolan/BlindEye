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

	UPROPERTY(EditAnywhere, Category=BlackBoard) 
	FBlackboardKeySelector IsFirstRunKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)  
	FBlackboardKeySelector IsDeadKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)   
	FBlackboardKeySelector IsFleeingKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsAttackingKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsImmobilizedKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard) 
	FBlackboardKeySelector ShrineKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)  
	FBlackboardKeySelector IsChannellingKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsGameEndedKey;

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
