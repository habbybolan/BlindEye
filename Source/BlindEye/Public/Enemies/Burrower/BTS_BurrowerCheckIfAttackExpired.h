// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_BurrowerCheckIfAttackExpired.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_BurrowerCheckIfAttackExpired : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackExpiredTime = 3;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsAttackingExpiredKey;
	
	UPROPERTY()
	float CurrTimer = 0;

	virtual void OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
