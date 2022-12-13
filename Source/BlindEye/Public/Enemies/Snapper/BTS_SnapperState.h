// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_SnapperState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_SnapperState : public UBTService
{
	GENERATED_BODY()

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category=BlackBoard) 
	FBlackboardKeySelector IsRagdolling;

	UPROPERTY(EditAnywhere, Category=BlackBoard)  
	FBlackboardKeySelector AttackState;
	
	UPROPERTY(EditAnywhere, Category=BlackBoard)  
	FBlackboardKeySelector IsAttackingShrine;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector ShrineAttackObjectKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector ShrineAttackPositionKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsGameEndedKey;
};
